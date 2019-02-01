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

#ifndef PLUGIN_H
#define PLUGIN_H

struct stPlugin {
  bool active;		// activation status
  int rate;		// sampling rate
  mydsp *dsp;	// dsp generated from FAUST code
  float* inputs[2];	// audio buffers
  float* outputs[2];
  
  // Needed host features.
  LV2_URID_Map* map;	// the urid extension
  LV2_URID_Unmap* unmap;	// the urid extension
  
  LV2_Atom_Forge forge;  // Forge for writing atoms in run thread
  LV2_Atom_Forge_Frame notify_frame;
  
  LV2_Worker_Schedule* schedule;
  
  // Structure for URI mapping
  stTubeAmpURIs uris;
  
  char *profile_file; // Current profile file name
  
  float *drybuf_l;     // Buffers for cabinet simulation bypass
  float *drybuf_r;
  
  float *preamp_inp_buf;  // Buffers for preamp convolver
  float *preamp_outp_buf;
  
  stPlugin(const int sr)
  {
    dsp = new mydsp();
    
    drybuf_l = new float[100000];
    drybuf_r = new float[100000];
    preamp_inp_buf = new float[100000];
    preamp_outp_buf = new float[100000];
    
    profile_file = new char[10000];
    
    active = false;
    rate = sr;

    map = NULL;

    dsp->init(rate);
  }

  ~stPlugin()
  {
    delete dsp;
    delete drybuf_l;
    delete drybuf_r;
    delete preamp_inp_buf;
    delete preamp_outp_buf;
    
    delete profile_file;
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
    if ((!active) | (blocksz < fragm))
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
      memcpy (profile_and_convolvers->preamp_convproc->inpdata(0), preamp_inp_buf+bufp, fragm * sizeof(float));
            
      profile_and_convolvers->preamp_convproc->process (true);
        
      memcpy (preamp_outp_buf+bufp, profile_and_convolvers->preamp_convproc->outdata(0), fragm * sizeof(float));
        
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

    memcpy (drybuf_l, outputs[0], blocksz * sizeof(float));
    memcpy (drybuf_r, outputs[1], blocksz * sizeof(float));
        
    while (bufp < blocksz)
    {
      memcpy (profile_and_convolvers->convproc->inpdata(0), outputs[0]+bufp, fragm * sizeof(float));
      memcpy (profile_and_convolvers->convproc->inpdata(1), outputs[1]+bufp, fragm * sizeof(float));
              
      profile_and_convolvers->convproc->process (true);
      memcpy (outputs[0]+bufp, profile_and_convolvers->convproc->outdata(0), fragm * sizeof(float));
      memcpy (outputs[1]+bufp, profile_and_convolvers->convproc->outdata(1), fragm * sizeof(float));
        
      bufp += fragm;
    }
        
    // Mix convolver output with bypassed signal
    for (int i=0;i<blocksz;i++)
    {
      outputs[0][i] = outputs[0][i]*(*ports.cabinet)+drybuf_l[i]*(1.0-*ports.cabinet); 
      outputs[1][i] = outputs[1][i]*(*ports.cabinet)+drybuf_r[i]*(1.0-*ports.cabinet); 
    }
  }
   
  // Function loads profile from file at 'path'
  // and creates new convolvers
  // with IR data from that *.tapf file
  void load_profile(const char *path, char *plugin_profile_file, int rate, stProfileAndConvolvers **p_profile)
  {
    (*p_profile) = new stProfileAndConvolvers;
    (*p_profile)->profile = new st_profile;
   
    FILE * profile_file= fopen(path, "rb");
    if (profile_file != NULL) 
    {
      strcpy(plugin_profile_file, path);
      
      if (fread((*p_profile)->profile, sizeof(st_profile), 1, profile_file) == 1)
      {
        float *preamp_temp_buffer;
        float *preamp_impulse;
      
        float *left_temp_buffer = NULL;
        float *right_temp_buffer = NULL;
      
        float * left_impulse;
        float * right_impulse;
      
        // IRs in *.tapf are 48000 Hz,
        // calculate ratio for resampling
        int ratio = rate/48000;
      
        st_impulse preamp_impheader, impheader;
        
        // Load preamp IR data to temp buffer
        if (fread(&preamp_impheader, sizeof(st_impulse), 1, profile_file) != 1)
        {
          return;
        }
        preamp_temp_buffer = new float[preamp_impheader.sample_count];
        if (fread(preamp_temp_buffer, sizeof(float), 
                  preamp_impheader.sample_count,
                  profile_file) != (size_t)preamp_impheader.sample_count)
        {
          return;
        }
        
        // Load cabsym IR data to temp buffers
        for (int i=0;i<2;i++)
        {
          if (fread(&impheader, sizeof(st_impulse), 1, profile_file) != 1)
          {
            return;
          }
          
          if (impheader.channel==0)
          {
            left_temp_buffer = new float[impheader.sample_count];
            if (fread(left_temp_buffer, sizeof(float),
                impheader.sample_count, profile_file) != (size_t)impheader.sample_count)
            {
              return;
            }
          }
          if (impheader.channel==1)
          {
            right_temp_buffer = new float[impheader.sample_count];
            if (fread(right_temp_buffer, sizeof(float),
                impheader.sample_count, profile_file) != (size_t)impheader.sample_count)
            {
              return;
            }
          }
        }
        
        if (rate==48000)
        {
          left_impulse = left_temp_buffer;
          right_impulse = right_temp_buffer;
          preamp_impulse = preamp_temp_buffer;
        }
        // If current rate is not 48000 Hz do resampling
        // with Zita-resampler
        else
        {
          preamp_impulse = new float[preamp_impheader.sample_count*ratio];
                
          Resampler *resampl = new Resampler();
          resampl->setup(48000,rate,1,48);
                
          int k = resampl->inpsize();
                
          float *preamp_in = new float[preamp_impheader.sample_count + k/2 - 1 + k - 1];
          float *preamp_out = new float[(preamp_impheader.sample_count + k/2 - 1 + k - 1)*ratio];
          
          // Create paddig before and after signal, needed for zita-resampler
          for (int i = 0; i < preamp_impheader.sample_count + k/2 - 1 + k - 1; i++)
          {
            preamp_in[i] = 0.0;
          }
              
          for (int i = k/2 - 1; i < preamp_impheader.sample_count + k/2 - 1; i++)
          {
            preamp_in[i] = preamp_temp_buffer[i - k/2 + 1];
          }
                
          resampl->inp_count = preamp_impheader.sample_count + k/2 - 1 + k - 1;
          resampl->out_count = (preamp_impheader.sample_count + k/2 - 1 + k - 1)*ratio;
          resampl->inp_data = preamp_in;
          resampl->out_data = preamp_out;
                
          resampl->process();
                
          for (int i=0;i<preamp_impheader.sample_count*ratio;i++)
          {
            preamp_impulse[i] = preamp_out[i] / ratio;
          }

          delete preamp_temp_buffer;
          delete preamp_out;
          delete preamp_in;
          
          delete resampl;
                
          left_impulse = new float[impheader.sample_count * ratio];
          right_impulse = new float[impheader.sample_count * ratio];
                
          float *inp_data;
          float *out_data;
                
          resampl = new Resampler();
          resampl->setup(48000,rate,2,48);
                
          k = resampl->inpsize();
                
          inp_data = new float[(impheader.sample_count + k/2 - 1 + k - 1)*2];
          
          // Create paddig before and after signal, needed for zita-resampler
          for (int i = 0; i < impheader.sample_count + k/2 - 1 + k - 1; i++)
          {
            inp_data[i*2] = 0.0;
            inp_data[i*2+1] = 0.0;
          }
                
          for (int i = k/2 - 1; i < impheader.sample_count + k/2 - 1; i++)
          {
            inp_data[i*2] = left_temp_buffer[i-k/2+1];
            inp_data[i*2+1] = right_temp_buffer[i-k/2+1];
          }
                
          out_data = new float[(impheader.sample_count + k/2 - 1 + k - 1)*ratio*2];
                
          resampl->inp_count = impheader.sample_count + k/2 - 1 + k - 1;
          resampl->out_count = (impheader.sample_count + k/2 - 1 + k - 1)*ratio;
          resampl->inp_data = inp_data;
          resampl->out_data = out_data;
                
          resampl->process();
                
          for (int i=0;i<impheader.sample_count*ratio;i++)
          {
            left_impulse[i] = out_data[i*2]/ratio;
            right_impulse[i] = out_data[i*2+1]/ratio;
          }
                
          delete inp_data;
          delete out_data;
          delete right_temp_buffer;
          delete left_temp_buffer;
                
          delete resampl;
                
        }
        
        // Create preamp convolver
        (*p_profile)->preamp_convproc = new Convproc;
        (*p_profile)->preamp_convproc->configure (1, 1, preamp_impheader.sample_count*ratio, fragm, fragm, Convproc::MAXPART, 0.0);
        (*p_profile)->preamp_convproc->impdata_create (0, 0, 1, preamp_impulse,
                                      0, preamp_impheader.sample_count*ratio);
        
        (*p_profile)->preamp_convproc->start_process(CONVPROC_SCHEDULER_PRIORITY, CONVPROC_SCHEDULER_CLASS);
        
        // Create cabsym convolver          
        (*p_profile)->convproc = new Convproc;
        (*p_profile)->convproc->configure (2, 2, 48000/2, fragm, fragm, Convproc::MAXPART, 0.0);
      
        (*p_profile)->convproc->impdata_create (0, 0, 1, left_impulse, 0, 48000/2);
        (*p_profile)->convproc->impdata_create (1, 1, 1, right_impulse, 0, 48000/2);
        
        (*p_profile)->convproc->start_process (CONVPROC_SCHEDULER_PRIORITY, CONVPROC_SCHEDULER_CLASS);
      
        fclose(profile_file);
        
      }
    }
  }
};

#endif
