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
 * This plugin is a booster/overdrive pedal emulator with equalizer.
 * When the _voice_ knob is in the left position the pedal works as a booster
 * When the _voice_ knob is in the right position the pedal works as a "tube screamer"
 * 
 * Process chain:
 *    
 * input->pre_filter->*drive_knob->overdrive->equalizer->post-filter->*volume_knob->output
 *      ->---voice--->                                 ->---voice---->
 *
 *
 *  pre-filter - highpass, 1 order, 720 Hz. Bypassed when _voice_ is in right position.
 *  overdrive - nonlinear element, emulation of the push-pull tube amplifier.
 *  equalizer - tonestack, bass-middle-treble.
 *  post-filter - lowpass, 1 order, 720 Hz. Bypassed when _voice_ is in right position.
 */

declare name "kpp_bluedream";
declare author "Oleg Kapitonov";
declare license "GPLv3";
declare version "0.1b";

import("stdfaust.lib"); 

process = output with {
    
    // Bypass button, 0 - pedal on, 1 -pedal off (bypass on)
    bypass = vslider("99_bypass", 0, 0, 1, 0.5);
          
    drive = vslider("drive",63,0,100,0.01);
    volume = vslider("volume",0.5,0,1,0.001);
    voice = vslider("voice",0.5,0,1,0.001);

    tonestack_low = vslider("bass",-6.9,-15,15,0.1);
    tonestack_middle = vslider("middle",-3.9,-15,15,0.1);
    tonestack_high = vslider("treble",0.6,-15,15,0.1);
    
    tonestack_low_freq = 20;
    tonestack_middle_freq = 500;
    tonestack_high_freq = 10000;
    
    tonestack_low_band = 400;
    tonestack_middle_band = 700;
    tonestack_high_band = 18000;
    
    /*-----Model of the tube in a push-pull amplifier-------*/
    
    // Bias of each half-wave so that they better match
    bias = 0.2;
    
    // Grid resistor
    Rg = 50000;
    
    // Grid capacitor
    Cgb = 100;
    
    // Distortion threshold, if the signal is bigger
    // it starts to get distorted
    Upor = 0.2;
    
    // Softness of distortion
    Kreg = 0.427;
    
    tau = 1/ma.SR;
    Cg = Cgb*1e-9;
    
    // Grid leak resistor (between the grid and the ground)
    Remax = 200000;
    
    // Model of the tube cascade, 0.5 is a hard clipping threshold    
    tube(Rg,Cg,Kreg,Upor,bias,cut) = main ~ _ : min(0.5) : +(bias) : max(cut) with {
        Ks(x) = 1/(max((x-Upor)*(Kreg),0)+1);
        Ksplus(x) = Upor - x*Upor;
        main(Uout,Uin) = (Ug * Ks(Ug) + Ksplus(Ks(Ug))) * Remax/(Remax+Rg) with {
            Ug = Uin - Uc letrec {
                'Uc = Uc + (Uin - Uc - Uout)/Rg/Cg*tau;
            };
        };
    };
    
    /*--------Processing chain-----------------*/
   
    // Used 2 tubes - for positive and negative half-waves (push-pull).
    // Stereo input and output, but during processing the signal is
    // converted to mono.
    
    stage_stomp = _ <: (fi.highpass(1,720)*(1-voice)),*(voice) : + : fi.lowpass(1,9000) : _<: 
    _,*(-1.0) : tube(Rg,Cg,Kreg,Upor,bias,0), tube(Rg,Cg,Kreg,Upor,bias,0) : - : 
    fi.peak_eq(tonestack_low,tonestack_low_freq,tonestack_low_band) : 
    fi.peak_eq(tonestack_middle,tonestack_middle_freq,tonestack_middle_band) : 
    fi.peak_eq(tonestack_high,tonestack_high_freq,tonestack_high_band) <: (fi.lowpass(1,720)*(1-
    voice)),*(voice) : +;
    
    stomp = fi.highpass(1,20) : *(ba.db2linear(drive*0.4))  : stage_stomp : 
    *(ba.db2linear(volume*60.0)/100.0) : *(1.0 - bypass);
    
    bypassed = *(bypass);
    
    output = _,_ : + <: stomp, bypassed : + <: _,_;
    
};


