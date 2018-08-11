/*
 * Copyright (C) 2018 Oleg Kapitonov
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
 
/*
 * This plugin is a distortion pedal emulator with equalizer.
 * 
 * Process chain:
 *    
 * input->pre_filter->*drive_knob->distortion->equalizer->post-filter->*volume_knob->output
 *
 *
 *  pre-filter - highpass, 1 order, 50-3900 Hz. The frequency depends
 *  on the _voice_ knob.
 *
 *  distortion - nonlinear element, hard clipper.
 *  
 *  equalizer - tonestack, bass-middle-treble. Middle lowered by 15 dB,
 *  Bass increased by 15 dB. This creates a sound characteristic 
 *  of the "distortion" pedals.
 *
 *  post-filter - lowpass, 1 order, 720 Hz.
 */

declare name "kpp_distruction";
declare author "Oleg Kapitonov";
declare license "GPLv3";
declare version "0.1b";

import("stdfaust.lib"); 

process = output with {
          
    drive = vslider("drive",63,0,100,0.01);
    volume = vslider("volume",0.5,0,1,0.001);
    voice = vslider("voice",0.5,0,1,0.001);
    
    tonestack_low = vslider("bass",0,-15,15,0.1);
    tonestack_middle = vslider("middle",0,-15,15,0.1);
    tonestack_high = vslider("treble",0,-15,15,0.1);
    
    tonestack_low_freq = 20;
    tonestack_middle_freq = 500;
    tonestack_high_freq = 10000;
    
    tonestack_low_band = 400;
    tonestack_middle_band = 700;
    tonestack_high_band = 18000;
    
    /*--------Processing chain-----------------*/
    
    // Distortion threshold, bigger signal is cutting
    Upor = 0.2;
    
    // Pre-filter frequency
    filt_freq = 50*(1-voice) + 3900*voice;
    
    stage_stomp = fi.highpass(1,filt_freq) : fi.lowpass(1,5200) : _<: _,*(-1.0) : (max(0.0) : min(Upor)), (max(0.0) : min(Upor)) : - : fi.peak_eq(tonestack_low+15.0,tonestack_low_freq,tonestack_low_band) : fi.peak_eq(tonestack_middle-15.0,tonestack_middle_freq,tonestack_middle_band) : 
    fi.peak_eq(tonestack_high,tonestack_high_freq,tonestack_high_band) : fi.lowpass(1,720);
    
    stomp = *(drive) : *(5) : stage_stomp : *(volume) : /(1.5) ;
    
    output = _ : stomp : _ ;
    
};


