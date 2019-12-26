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

#include <string.h>
#include <math.h>
#include <map>
#include <stdint.h>
#include <sys/stat.h>
#include <unistd.h>

#include <lv2/lv2plug.in/ns/ext/atom/util.h>
#include <lv2/lv2plug.in/ns/ext/urid/urid.h>
#include "lv2/lv2plug.in/ns/ext/atom/forge.h"
#include "lv2/lv2plug.in/ns/ext/buf-size/buf-size.h"
#include "lv2/lv2plug.in/ns/ext/options/options.h"
#include "lv2/lv2plug.in/ns/ext/patch/patch.h"
#include "lv2/lv2plug.in/ns/ext/worker/worker.h"
#include "lv2/lv2plug.in/ns/ext/state/state.h"
#include "lv2/lv2plug.in/ns/lv2core/lv2.h"

#include "atom_sink.h"
#include "profile.h"
#include "kpp_tubeamp.h"
#include "faust-support.h"

using namespace std;

#define DEFAULT_PROFILE "American Clean"

// Defines for compatability with
// FAUST generated code

#define VOLUME_CTRL *(ports.volume)
#define DRIVE_CTRL *(ports.drive)
#define MASTERGAIN_CTRL *(ports.mastergain)

#define AMP_BIAS_CTRL profile->amp_bias
#define AMP_KREG_CTRL profile->amp_Kreg
#define AMP_UPOR_CTRL profile->amp_Upor

#define PREAMP_BIAS_CTRL profile->preamp_bias
#define PREAMP_KREG_CTRL profile->preamp_Kreg
#define PREAMP_UPOR_CTRL profile->preamp_Upor

#define LOW_CTRL *(ports.low)
#define MIDDLE_CTRL *(ports.middle)
#define HIGH_CTRL *(ports.high)

#define LOW_FREQ_CTRL profile->tonestack_low_freq
#define MIDDLE_FREQ_CTRL profile->tonestack_middle_freq
#define HIGH_FREQ_CTRL profile->tonestack_high_freq

#define LOW_BAND_CTRL profile->tonestack_low_band
#define MIDDLE_BAND_CTRL profile->tonestack_middle_band
#define HIGH_BAND_CTRL profile->tonestack_high_band

#define PREAMP_LEVEL profile->preamp_level
#define AMP_LEVEL profile->amp_level

#define SAG_TIME profile->sag_time
#define SAG_COEFF profile->sag_coeff

#define OUTPUT_LEVEL profile->output_level

// LV2 port numbers
enum {PORT_LOW, PORT_MIDDLE, PORT_HIGH, PORT_DRIVE, PORT_MASTERGAIN, PORT_VOLUME, PORT_CABINET,
    PORT_IN0, PORT_IN1, PORT_OUT0, PORT_OUT1, PORT_CONTROL, PORT_NOTIFY};

// FAUST generated code
#include "plugin.h"

// Check *.tapf file signature
int check_profile_file(const char *path)
{
  int status = 0;

  FILE * profile_file = fopen(path, "rb");

  if (profile_file != NULL)
  {
    st_profile_header check_profile;
    if (fread(&check_profile, sizeof(st_profile_header), 1, profile_file) == 1)
    {
      if (!strncmp(check_profile.signature, "TaPf", 4))
      {
        status = 1;
      }
    }
    else status = 1;

    fclose(profile_file);
  }

  return status;
}

// LV2 Callback functions

static LV2_Handle
instantiate(const LV2_Descriptor*     descriptor,
            double                    rate,
            const char*               bundle_path,
            const LV2_Feature* const* features)
{
  stPlugin* plugin = new stPlugin((int)rate);
  const LV2_Options_Option* options = NULL;
  // Scan host features for URID map.
  for (int i = 0; features[i]; i++)
  {
    if (!strcmp(features[i]->URI, LV2_URID_URI "#map"))
    {
      plugin->map = (LV2_URID_Map*)features[i]->data;
    }
    else if (!strcmp(features[i]->URI, LV2_URID_URI "#unmap"))
    {
      plugin->unmap = (LV2_URID_Unmap*)features[i]->data;
    }
    else if (!strcmp(features[i]->URI, LV2_OPTIONS__options))
    {
      options = (const LV2_Options_Option*) features[i]->data;
    }
    else if (!strcmp(features[i]->URI, LV2_WORKER__schedule))
    {
      plugin->schedule = (LV2_Worker_Schedule*)features[i]->data;
    }
  }
  if (!plugin->map)
  {
    fprintf(stderr, "%s: host doesn't support urid:map, giving up\n", PLUGIN_URI);
    delete plugin;
    return 0;
  }
  if (!plugin->schedule)
  {
    fprintf(stderr, "%s: host doesn't support woker:schedule, giving up\n", PLUGIN_URI);
    delete plugin;
    return 0;
  }

  int32_t bufsize = 8192;
  LV2_URID bufsz_max = plugin->map->map(plugin->map->handle, LV2_BUF_SIZE__maxBlockLength);
  LV2_URID atom_Int = plugin->map->map(plugin->map->handle, LV2_ATOM__Int);
  for (const LV2_Options_Option* opt = options; opt->key; opt++)
  {
    if (opt->context == LV2_OPTIONS_INSTANCE
        && opt->key == bufsz_max && opt->type == atom_Int) {
      bufsize = *(const int32_t*) opt->value;
    }
  }
  plugin->set_bufsize(bufsize);

  plugin->uris.patch_Set = plugin->map->map(plugin->map->handle, LV2_PATCH__Set);
  plugin->uris.patch_Get = plugin->map->map(plugin->map->handle, LV2_PATCH__Get);
  plugin->uris.patch_property = plugin->map->map(plugin->map->handle, LV2_PATCH__property);
  plugin->uris.patch_value = plugin->map->map(plugin->map->handle, LV2_PATCH__value);
  plugin->uris.atom_Path = plugin->map->map(plugin->map->handle, LV2_ATOM__Path);
  plugin->uris.profile = plugin->map->map(plugin->map->handle, "https://faustlv2.bitbucket.io/kpp_tubeamp#profile");
  plugin->uris.freeSample = plugin->map->map(plugin->map->handle, "https://faustlv2.bitbucket.io/kpp_tubeamp#freeSample");

  lv2_atom_forge_init(&plugin->forge, plugin->map);

  plugin->bundle_path = bundle_path;

  return (LV2_Handle)plugin;
}

static void
cleanup(LV2_Handle instance)
{
  stPlugin* plugin = (stPlugin*)instance;
  delete plugin;
}

// Worker function (works in the separate thread)
// Can free old profile structures or load new
static LV2_Worker_Status
work(LV2_Handle                  instance,
     LV2_Worker_Respond_Function respond,
     LV2_Worker_Respond_Handle   handle,
     uint32_t                    size,
     const void*                 data)
{
  stPlugin* plugin = (stPlugin*)instance;
  const LV2_Atom* atom = (const LV2_Atom*)data;
  const char *path = NULL;

  if (atom->type == plugin->uris.freeSample)
  {
    // Free old profile
    stProfileMessage *msg = (stProfileMessage *)data;
    std::lock_guard<std::mutex> lock(plugin->profile_free_mutex);
    delete msg->body.profile;
  }
  else if (atom->type == plugin->forge.Object)
  {
    // Load new profile
    const LV2_Atom* value = NULL;
    lv2_atom_object_get((const LV2_Atom_Object*)atom, plugin->uris.patch_value, &value, 0);

    path = (const char*)LV2_ATOM_BODY_CONST(value);

    if (!path)
    {
      fprintf(stderr,"Error in path while loading profile!\n");
      return LV2_WORKER_ERR_UNKNOWN;
    }

    if (check_profile_file(path))
    {
      stProfile *profile = stPlugin::load_profile(path, plugin->rate);
      if (!profile)
      {
        fprintf(stderr,"Error while loading profile!\n");
        return LV2_WORKER_ERR_UNKNOWN;
      }
      stProfileMessage msg = {
        { sizeof(stProfileMessageBody), plugin->uris.patch_value },
        { profile }
      };

      respond(handle, lv2_atom_total_size((LV2_Atom*)&msg), (LV2_Atom*)&msg);
    }
  }

  return LV2_WORKER_SUCCESS;
}

// Response after worker function returns
static LV2_Worker_Status
work_response(LV2_Handle  instance,
              uint32_t    size,
              const void* data)
{
  stPlugin* plugin = (stPlugin*)instance;

  // Schedule work to free the old profile
  stProfileMessage freemsg = {
    { sizeof(stProfileMessageBody), plugin->uris.freeSample },
    { plugin->profile }
  };

  // Swap old and new profile structures
  stProfileMessage *msg = (stProfileMessage *)data;
  plugin->profile = msg->body.profile;
  plugin->dsp->profile = &msg->body.profile->header;
  plugin->convproc = &msg->body.profile->convproc;
  plugin->preamp_convproc = &msg->body.profile->preamp_convproc;
  // Schedule deleting of old profile structures
  plugin->schedule->schedule_work(plugin->schedule->handle, sizeof(freemsg), &freemsg);

  return LV2_WORKER_SUCCESS;
}

static void
connect_port(LV2_Handle instance,
             uint32_t   port,
             void*      data)
{
  stPlugin* plugin = (stPlugin*)instance;
  switch (port) {
      case PORT_LOW:
          plugin->dsp->ports.low = (float*) data;
          break;
      case PORT_MIDDLE:
          plugin->dsp->ports.middle = (float *) data;
          break;
      case PORT_HIGH:
          plugin->dsp->ports.high = (float *) data;
          break;
      case PORT_DRIVE:
          plugin->dsp->ports.drive = (float *) data;
          break;
      case PORT_MASTERGAIN:
          plugin->dsp->ports.mastergain = (float *) data;
          break;
      case PORT_VOLUME:
          plugin->dsp->ports.volume = (float *) data;
          break;
      case PORT_CABINET:
          plugin->dsp->ports.cabinet = (float *) data;
          break;
      case PORT_IN0:
          plugin->inputs[0] = (float *) data;
          break;
      case PORT_IN1:
          plugin->inputs[1] = (float *) data;
          break;
      case PORT_OUT0:
          plugin->outputs[0] = (float *) data;
          break;
      case PORT_OUT1:
          plugin->outputs[1] = (float *) data;
      break;
      case PORT_CONTROL:
          plugin->dsp->ports.control = (LV2_Atom_Sequence *) data;
      break;
      case PORT_NOTIFY:
          plugin->dsp->ports.notify = (LV2_Atom_Sequence *) data;
  }
}

static void
run(LV2_Handle instance, uint32_t n_samples)
{
  stPlugin* plugin = (stPlugin*)instance;
  // Process audio.
  plugin->process_audio(n_samples);

  const uint32_t notify_capacity = plugin->dsp->ports.notify->atom.size;
  lv2_atom_forge_set_buffer(&plugin->forge,
                            (uint8_t*)plugin->dsp->ports.notify,
                            notify_capacity);

  lv2_atom_forge_sequence_head(&plugin->forge, &plugin->notify_frame, 0);

  // Read Atom messages
  LV2_ATOM_SEQUENCE_FOREACH(plugin->dsp->ports.control, ev)
  {
    const LV2_Atom_Object* obj = (const LV2_Atom_Object*)&ev->body;

    if (obj->body.otype == plugin->uris.patch_Set)
    {
        // Received patch_Set message
        const LV2_Atom* property = NULL;
        lv2_atom_object_get(obj, plugin->uris.patch_property, &property, 0);

        if (((const LV2_Atom_URID*)property)->body == plugin->uris.profile)
        {
          // Received profile object
          const LV2_Atom* value = NULL;

          lv2_atom_object_get(obj, plugin->uris.patch_value, &value, 0);

          if (value->type == plugin->uris.atom_Path)
          {
            // Received object path, load new profile in worker
            plugin->schedule->schedule_work(plugin->schedule->handle,
                                                lv2_atom_total_size((LV2_Atom*)obj),
                                                (LV2_Atom*)obj);
          }
        }
    }
    else if (obj->body.otype == plugin->uris.patch_Get)
    {
      // Received patch_Get message,
      // send current profile path back
      lv2_atom_forge_frame_time(&plugin->forge, 0);
      LV2_Atom_Forge_Frame frame;
      lv2_atom_forge_object(&plugin->forge, &frame, 0, plugin->uris.patch_Set);

      lv2_atom_forge_key(&plugin->forge, plugin->uris.patch_property);
      lv2_atom_forge_urid(&plugin->forge, plugin->uris.profile);
      lv2_atom_forge_key(&plugin->forge, plugin->uris.patch_value);
      if (plugin->profile)
      {
        lv2_atom_forge_path(&plugin->forge,
            plugin->profile->path.data(), plugin->profile->path.size());
      }
      else
      {
        lv2_atom_forge_path(&plugin->forge, NULL, 0);
      }

      lv2_atom_forge_pop(&plugin->forge, &frame);
    }

  }
}

static void
activate(LV2_Handle instance)
{
  stPlugin* plugin = (stPlugin*)instance;
  plugin->resume();
}

static void
deactivate(LV2_Handle instance)
{
  stPlugin* plugin = (stPlugin*)instance;
  plugin->suspend();
}

// Used to save profile path
static LV2_State_Status
save(LV2_Handle                instance,
     LV2_State_Store_Function  store,
     LV2_State_Handle          handle,
     uint32_t                  flags,
     const LV2_Feature* const* features)
{
  stPlugin* plugin = (stPlugin*)instance;

  LV2_State_Map_Path* map_path = NULL;

  for (int i = 0; features[i]; i++) {
    if (!strcmp(features[i]->URI, LV2_STATE__mapPath)) {
      map_path = (LV2_State_Map_Path*)features[i]->data;
    }
  }

  if (!map_path) {
    return LV2_STATE_ERR_NO_FEATURE;
  }

  // Map absolute sample path to an abstract state path
  char *path = NULL;
  {
    std::lock_guard<std::mutex> lock(plugin->profile_free_mutex);
    if (plugin->profile)
    {
      path = map_path->abstract_path(map_path->handle, plugin->profile->path.c_str());
    }
  }

  // Store profile = abstract path
  store(handle,
        plugin->uris.profile,
        path,
        path ? strlen(path) + 1 : 0,
        plugin->uris.atom_Path,
        LV2_STATE_IS_POD | LV2_STATE_IS_PORTABLE);

  free(path);
  return LV2_STATE_SUCCESS;
}

// Used to restore saved profile path
static LV2_State_Status
restore(LV2_Handle                  instance,
        LV2_State_Retrieve_Function retrieve,
        LV2_State_Handle            handle,
        uint32_t                    flags,
        const LV2_Feature* const*   features)
{
  stPlugin* plugin = (stPlugin*)instance;

  LV2_State_Map_Path*  paths    = NULL;

  for (int i = 0; features[i]; i++)
  {
    if (!strcmp(features[i]->URI, LV2_STATE__mapPath))
    {
      paths = (LV2_State_Map_Path*)features[i]->data;
    }
  }

  if (!paths)
  {
    fprintf(stderr, "%s: host doesn't support state:map_Path\n", PLUGIN_URI);
    return LV2_STATE_ERR_NO_FEATURE;
  }

  // Get eg:sample from state
  size_t      size;
  uint32_t    type;
  uint32_t    valflags;
  const void* value = retrieve(handle, plugin->uris.profile,
                               &size, &type, &valflags);
  std::string path;
  if (!value)
  {
    path = plugin->bundle_path +  "/profiles/" DEFAULT_PROFILE ".tapf";
  }
  else if (type != plugin->uris.atom_Path)
  {
    return LV2_STATE_ERR_BAD_TYPE;
  }
  else
  {
    char *abspath = paths->absolute_path(paths->handle, (const char *) value);
    path = abspath;
    free(abspath);
  }

  // Map abstract state path to absolute path

  if (check_profile_file(path.c_str()))
  {
    // Send Atom message to load this profile
    LV2_Atom_Forge forge;
    LV2_Atom*      buf = (LV2_Atom*)calloc(1, path.size() + 128);
    lv2_atom_forge_init(&forge, plugin->map);
    lv2_atom_forge_set_sink(&forge, atom_sink, atom_sink_deref, buf);

    LV2_Atom_Forge_Frame frame;
    lv2_atom_forge_object(&forge, &frame, 0, plugin->uris.patch_Set);

    lv2_atom_forge_key(&forge, plugin->uris.patch_property);
    lv2_atom_forge_urid(&forge, plugin->uris.profile);
    lv2_atom_forge_key(&forge, plugin->uris.patch_value);
    lv2_atom_forge_path(&forge, path.data(), path.size());

    lv2_atom_forge_pop(&forge, &frame);

    const uint32_t msg_size = lv2_atom_pad_size(buf->size);
    plugin->schedule->schedule_work(plugin->schedule->handle, msg_size, buf + 1);
    free(buf);
  }

  return LV2_STATE_SUCCESS;
}


const void*
extension_data(const char* uri)
{
  static const LV2_State_Interface  state  = { save, restore };
  static const LV2_Worker_Interface worker = { work, work_response, NULL };
  if (!strcmp(uri, LV2_STATE__interface))
  {
    return &state;
  }
  else if (!strcmp(uri, LV2_WORKER__interface))
  {
    return &worker;
  }
  return NULL;
}

static const LV2_Descriptor descriptor = {
  PLUGIN_URI,
  instantiate,
  connect_port,
  activate,
  run,
  deactivate,
  cleanup,
  extension_data
};

extern "C"
LV2_SYMBOL_EXPORT
const LV2_Descriptor*
lv2_descriptor(uint32_t index)
{
  switch (index) {
  case 0:
    return &descriptor;
  default:
    return NULL;
  }
}
