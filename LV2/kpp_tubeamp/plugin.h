/*
 * Copyright (C) 2019 Oleg Kapitonov
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 * --------------------------------------------------------------------------
 */

#include <memory>
#include <mutex>
#include <vector>

#include "lv2/lv2plug.in/ns/ext/worker/worker.h"

#include "kpp_tubeamp.h"

// Zita-convolver parameters
#define CONVPROC_SCHEDULER_PRIORITY 0
#define CONVPROC_SCHEDULER_CLASS SCHED_FIFO
#define THREAD_SYNC_MODE true

#define fragm 64

#include "kpp_tubeamp_dsp.h"

struct stPlugin {
  bool active;       // activation status
  int rate;          // sampling rate
  mydsp *dsp;        // dsp generated from FAUST code
  float* inputs[2];  // audio buffers
  float* outputs[2];

  // Needed host features.
  LV2_URID_Map* map;  // the urid extension
  LV2_URID_Unmap* unmap;  // the urid extension

  LV2_Atom_Forge forge;  // Forge for writing atoms in run thread
  LV2_Atom_Forge_Frame notify_frame;

  LV2_Worker_Schedule* schedule;

  // Structure for URI mapping
  stTubeAmpURIs uris;

  std::string bundle_path;

  std::vector<float> drybuf_l;     // Buffers for cabinet simulation bypass
  std::vector<float> drybuf_r;

  std::vector<float> preamp_inp_buf;  // Buffers for preamp convolver
  std::vector<float> preamp_outp_buf;

  stProfile *profile = NULL;
  Convproc *preamp_convproc = NULL;
  Convproc *convproc = NULL;

  std::mutex profile_free_mutex;

  stPlugin(const int sr)
  {
    dsp = new mydsp();

    active = false;
    rate = sr;

    map = NULL;

    dsp->init(rate);
  }

  ~stPlugin()
  {
    delete dsp;
  }

  void set_bufsize(int size)
  {
    drybuf_l.resize(size);
    drybuf_r.resize(size);
    preamp_inp_buf.resize(size);
    preamp_outp_buf.resize(size);
  }

  // Plugin activation status. suspend() deactivates a plugin (disables audio
  // processing), resume() reactivates it. Also, set_rate() changes the sample
  // rate.

  void suspend()
  {
    active = false;
  }

  void resume()
  {
    dsp->init(rate);
    active = true;
  }

  void set_rate(int sr)
  {
    rate = sr;
    dsp->init(rate);
  }

  // Audio processing function
  void process_audio(int blocksz)
  {
    AVOIDDENORMALS;
    if ((!active) || (blocksz < fragm) || !convproc || !preamp_convproc)
    {
      // If plugin is not active or block
      // size too small - simply copy input
      for (int i = 0; i < 2; i++)
      {
        for (int j = 0; j < blocksz; j++) outputs[i][j] = inputs[i][j];
      }

      return;
    }
    // Preamp convolver
    for (int i=0; i<blocksz; i++)
    {
      preamp_inp_buf[i] = (inputs[0][i] + inputs[1][i])/2.0;
    }

    // Zita-convolver accepts 'fragm' number of samples,
    // real buffer size may be greater,
    // so perform convolution in multiple steps
    int bufp = 0;

    while (bufp < blocksz)
    {
      memcpy (preamp_convproc->inpdata(0), preamp_inp_buf.data()+bufp, fragm * sizeof(float));

      preamp_convproc->process (true);

      memcpy (preamp_outp_buf.data()+bufp, preamp_convproc->outdata(0), fragm * sizeof(float));

      bufp += fragm;
    }
    // Return result of convolution
    // back to input buffer
    for (int i=0; i<blocksz; i++)
    {
      inputs[0][i] = preamp_outp_buf[i];
      inputs[1][i] = preamp_outp_buf[i];
    }

    // Apply main tubeAmp model from FAUST code
    dsp->compute(blocksz, inputs, outputs);
    // Cabinet simulation convolver
    bufp = 0;

    memcpy (drybuf_l.data(), outputs[0], blocksz * sizeof(float));
    memcpy (drybuf_r.data(), outputs[1], blocksz * sizeof(float));

    while (bufp < blocksz)
    {
      memcpy (convproc->inpdata(0), outputs[0]+bufp, fragm * sizeof(float));
      memcpy (convproc->inpdata(1), outputs[1]+bufp, fragm * sizeof(float));

      convproc->process (true);
      memcpy (outputs[0]+bufp, convproc->outdata(0), fragm * sizeof(float));
      memcpy (outputs[1]+bufp, convproc->outdata(1), fragm * sizeof(float));

      bufp += fragm;
    }

    // Mix convolver output with bypassed signal
    for (int i=0;i<blocksz;i++)
    {
      outputs[0][i] = outputs[0][i]*(*dsp->ports.cabinet)+drybuf_l[i]*(1.0-*dsp->ports.cabinet);
      outputs[1][i] = outputs[1][i]*(*dsp->ports.cabinet)+drybuf_r[i]*(1.0-*dsp->ports.cabinet);
    }
  }

  // Function loads profile from file at 'path'
  // and creates new convolvers
  // with IR data from that *.tapf file
  static stProfile *load_profile(const char *path, int rate)
  {

    FILE *profile_file = fopen(path, "rb");
    if (profile_file != NULL)
    {
      std::unique_ptr<stProfile> p_profile(new stProfile);

      if (fread(&p_profile.get()->header, sizeof(st_profile_header), 1, profile_file) == 1)
      {

        // IRs in *.tapf are 48000 Hz,
        // calculate ratio for resampling
        float ratio = (float)rate / 48000.0;

        st_impulse_header preamp_impheader, impheader;

        // Load preamp IR data to temp buffer
        if (fread(&preamp_impheader, sizeof(st_impulse_header), 1, profile_file) != 1)
        {
          return NULL;
        }
        std::vector<float> preamp_impulse(preamp_impheader.sample_count);
        if (fread(preamp_impulse.data(), sizeof(float),
                  preamp_impheader.sample_count,
                  profile_file) != (size_t)preamp_impheader.sample_count)
        {
          return NULL;
        }

        std::vector<float> left_impulse;
        std::vector<float> right_impulse;
        // Load cabsym IR data to temp buffers
        for (int i=0;i<2;i++)
        {
          if (fread(&impheader, sizeof(st_impulse_header), 1, profile_file) != 1)
          {
            return NULL;
          }

          if (impheader.channel==0)
          {
            left_impulse.resize(impheader.sample_count);
            if (fread(left_impulse.data(), sizeof(float),
                impheader.sample_count, profile_file) != (size_t)impheader.sample_count)
            {
              return NULL;
            }
          }
          if (impheader.channel==1)
          {
            right_impulse.resize(impheader.sample_count);
            if (fread(right_impulse.data(), sizeof(float),
                impheader.sample_count, profile_file) != (size_t)impheader.sample_count)
            {
              return NULL;
            }
          }
        }

        // If current rate is not 48000 Hz do resampling
        // with Zita-resampler
        if (rate!=48000)
        {
          {
            Resampler resampl;
            resampl.setup(48000,rate,1,48);

            int k = resampl.inpsize();

            std::vector<float> preamp_in(preamp_impheader.sample_count + k/2 - 1 + k - 1);
            std::vector<float> preamp_out((uint)((preamp_impheader.sample_count + k/2 - 1 + k - 1)*ratio));

            // Create paddig before and after signal, needed for zita-resampler
            for (int i = 0; i < preamp_impheader.sample_count + k/2 - 1 + k - 1; i++)
            {
              preamp_in[i] = 0.0;
            }

            for (int i = k/2 - 1; i < preamp_impheader.sample_count + k/2 - 1; i++)
            {
              preamp_in[i] = preamp_impulse[i - k/2 + 1];
            }

            resampl.inp_count = preamp_impheader.sample_count + k/2 - 1 + k - 1;
            resampl.out_count = (uint)((preamp_impheader.sample_count + k/2 - 1 + k - 1)*ratio);
            resampl.inp_data = preamp_in.data();
            resampl.out_data = preamp_out.data();

            resampl.process();

            preamp_impulse.resize(ratio);
            for (uint i = 0; i < (uint)(preamp_impheader.sample_count*ratio); i++)
            {
              preamp_impulse[i] = preamp_out[i] / ratio;
            }
          }

          {
            Resampler resampl;
            resampl.setup(48000,rate,2,48);

            int k = resampl.inpsize();

            std::vector<float> inp_data((impheader.sample_count + k/2 - 1 + k - 1)*2);

            // Create paddig before and after signal, needed for zita-resampler
            for (int i = 0; i < impheader.sample_count + k/2 - 1 + k - 1; i++)
            {
              inp_data[i*2] = 0.0;
              inp_data[i*2+1] = 0.0;
            }

            for (int i = k/2 - 1; i < impheader.sample_count + k/2 - 1; i++)
            {
              inp_data[i*2] = left_impulse[i-k/2+1];
              inp_data[i*2+1] = right_impulse[i-k/2+1];
            }

            std::vector<float> out_data((uint)((impheader.sample_count + k/2 - 1 + k - 1)*ratio*2));

            resampl.inp_count = impheader.sample_count + k/2 - 1 + k - 1;
            resampl.out_count = (uint)((impheader.sample_count + k/2 - 1 + k - 1)*ratio);
            resampl.inp_data = inp_data.data();
            resampl.out_data = out_data.data();

            resampl.process();

            left_impulse.resize((uint)(impheader.sample_count * ratio));
            right_impulse.resize((uint)(impheader.sample_count * ratio));

            for (uint i = 0; i < (uint)(impheader.sample_count*ratio); i++)
            {
              left_impulse[i] = out_data[i*2] / ratio;
              right_impulse[i] = out_data[i*2+1] / ratio;
            }
          }

        }

        // Create preamp convolver
        Convproc *p_preamp_convproc = &p_profile->preamp_convproc;
        p_preamp_convproc->configure (1, 1, (uint)(preamp_impheader.sample_count*ratio),
                                      fragm, fragm, Convproc::MAXPART, 0.0);
        p_preamp_convproc->impdata_create (0, 0, 1, preamp_impulse.data(),
                                      0, (uint)(preamp_impheader.sample_count*ratio));

        p_preamp_convproc->start_process(CONVPROC_SCHEDULER_PRIORITY,
                                         CONVPROC_SCHEDULER_CLASS);

        // Create cabsym convolver
        Convproc *p_convproc = &p_profile->convproc;
        p_convproc->configure (2, 2, 48000/2, fragm, fragm, Convproc::MAXPART, 0.0);

        p_convproc->impdata_create (0, 0, 1, left_impulse.data(), 0, 48000/2);
        p_convproc->impdata_create (1, 1, 1, right_impulse.data(), 0, 48000/2);

        p_convproc->start_process (CONVPROC_SCHEDULER_PRIORITY, CONVPROC_SCHEDULER_CLASS);

        fclose(profile_file);

        p_profile->path = path;
        return p_profile.release();
      }
    }
    return NULL;
  }
};
