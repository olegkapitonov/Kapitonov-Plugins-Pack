/*
 * Copyright (C) 2018-2020 Oleg Kapitonov
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
 *  pre-filter - highpass, 1 order, 720 Hz,
 *  lowpass, 1 order, 1200 Hz
 * 
 *  Voice knob disables pre-filter in left position.
 *
 *  distortion - nonlinear element, hard clipper.
 *  
 *  equalizer - tonestack, bass-middle-treble.
 *
 *  post-filter - lowpass, 1 order, 1220 Hz,
 *                highpass, 1 order, 70 Hz
 */

declare name "kpp_distruction";
declare author "Oleg Kapitonov";
declare license "GPLv3";
declare version "1.1";

import("stdfaust.lib"); 

process = output with {

    // Bypass button, 0 - pedal on, 1 -pedal off (bypass on)
    bypass = checkbox("99_bypass");
          
    drive = vslider("drive",63,0,100,0.01);
    volume = vslider("volume",0.5,0,1,0.001);
    voice = vslider("voice",0.5,0,1,0.001);
    
    tonestack_low = vslider("bass",0,-15,15,0.1);
    tonestack_middle = vslider("middle",0,-15,15,0.1);
    tonestack_high = vslider("treble",0,-15,15,0.1);
    
    tonestack_low_freq = 100;
    tonestack_middle_freq = 700;
    tonestack_high_freq = 3300;
    
    tonestack_low_band = 200;
    tonestack_middle_band = 700;
    tonestack_high_band = 2000;
    
    clamp = min(2.0) : max(-2.0);
    
    // Distortion threshold, bigger signal is cutting
    Upor = 0.2;
    
    // Bias of each half-wave so that they better match
    bias = 0.2;

    
    pre_filter = fi.lowpass(1, 1200) <:
    fi.highpass(1, 720),_ :
    *(voice),*(1 - voice) : +;
    
    post_filter = fi.lowpass(1, 1220) : fi.highpass(1,70);
    
    // Softness of distortion
    Kreg = 1.0;
    
    tube(Kreg,Upor,bias,cut) = main : +(bias) : max(cut) with {
        Ks(x) = 1/(max((x-Upor)*(Kreg),0)+1);
        Ksplus(x) = Upor - x*Upor;
        main(Uin) = (Uin * Ks(Uin) + Ksplus(Ks(Uin)));
    };
    
    
    stage_stomp = pre_filter : _<: 
    _,*(-1.0) : tube(Kreg,Upor,bias,0), tube(Kreg,Upor,bias,0) : - :
    fi.peak_eq(tonestack_low,tonestack_low_freq,tonestack_low_band) :
    fi.peak_eq(tonestack_middle - 6,tonestack_middle_freq,tonestack_middle_band) : 
    fi.peak_eq(tonestack_high,tonestack_high_freq,tonestack_high_band) :
    post_filter :
    clamp;
    
    stomp = fi.dcblocker : clamp : *(ba.db2linear(drive * 60.0 / 100.0)-1) :
    *(5) : stage_stomp : *((ba.db2linear(volume * 15.0)-1) / 100.0) : fi.dcblocker;
    
    output = _,_ : + : ba.bypass1(bypass, stomp) <: _,_;
    
};


