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

#ifndef PROFILE_H
#define PROFILE_H

// Structure with tubeAmp model
// parameters.

typedef struct
{
    char signature[4];
    
    uint32_t version;
    
    float preamp_level;
    float preamp_bias;
    float preamp_Kreg;
    float preamp_Upor;
    
    float tonestack_low_freq;
    float tonestack_low_band;
    float tonestack_middle_freq;
    float tonestack_middle_band;
    float tonestack_high_freq;
    float tonestack_high_band;
    
    float amp_level;
    float amp_bias;
    float amp_Kreg;
    float amp_Upor;
    
    float sag_time;
    float sag_coeff;
    
    float output_level;
    
}st_profile;

// Header structure of
// impulse response data
// in *.tapf profile file

typedef struct {
    int sample_rate;
    int channel;
    int sample_count;
}st_impulse;

#endif
