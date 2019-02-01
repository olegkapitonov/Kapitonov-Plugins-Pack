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


#ifndef KPP_TUBEAMP_H
#define KPP_TUBEAMP_H


/**
 * On Intel set FZ (Flush to Zero) and DAZ (Denormals Are Zero)
 * flags to avoid costly denormals.
 */

#ifdef __SSE__
    #include <xmmintrin.h>
    #ifdef __SSE2__
        #define AVOIDDENORMALS _mm_setcsr(_mm_getcsr() | 0x8040)
    #else
        #define AVOIDDENORMALS _mm_setcsr(_mm_getcsr() | 0x8000)
    #endif
#else
    #define AVOIDDENORMALS
#endif

// Structure holds pointers
// to LV2 plugin ports
struct stPorts 
{
    float *volume;
    float *drive;
    float *low;
    float *middle;
    float *high;
    float *mastergain;
    float *cabinet;

    LV2_Atom_Sequence* control;
    LV2_Atom_Sequence* notify;
};

// Structure for URI mapping
struct stTubeAmpURIs
{
	LV2_URID patch_Get;
	LV2_URID patch_Set;
  LV2_URID patch_property;
  LV2_URID patch_value;
  LV2_URID profile;
  LV2_URID atom_Path;
  LV2_URID freeSample;
};

struct stProfileAndConvolvers
{
  st_profile *profile;
  Convproc *convproc;
  Convproc *preamp_convproc;
};

// Structure of Atom message
// to free old profile data
struct stProfileMessage
{
	LV2_Atom atom;
	stProfileAndConvolvers*  profile_and_convolvers;
};

// Needed for compatability with FAUST generated code
struct Meta : std::map<const char*, const char*>
{
  void declare(const char *key, const char *value)
  {
    (*this)[key] = value;
  }
  const char* get(const char *key, const char *def)
  {
    if (this->find(key) != this->end())
      return (*this)[key];
    else
      return def;
  }
};

#endif
