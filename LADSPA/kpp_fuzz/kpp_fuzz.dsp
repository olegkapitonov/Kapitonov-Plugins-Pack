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
 
/*
 * This plugin is a vintage fuzz pedal emulator.
 * 
 * Process chain:
 *    
 * input->pre_filter->*drive_knob->fuzz->tone->*volume_knob->output
 *
 *  pre-filter - lowpass, 1 order, 1720 Hz. Emulates effect
 *  of low impedance of vintage pedal.
 * 
 *  distortion - 2 cascades, asymmetric quadratic distortion and clipper.
 *  Emulates distortion of old bad class A transistor cascades.
 *  tone - highshelf 720 Hz.
 */

declare name "kpp_fuzz";
declare author "Oleg Kapitonov";
declare license "GPLv3";
declare version "1.0RC1";

import("stdfaust.lib"); 

process = output with {
    
    // Bypass button, 0 - pedal on, 1 -pedal off (bypass on)
    bypass = checkbox("99_bypass");
          
    fuzz = vslider("fuzz",0,0,100,0.01);
    tone = vslider("tone",0,-15,0,0.1);
    volume = vslider("volume",0.5,0,1,0.001);
    
    
    clamp = min(2.0) : max(-2.0);
    
    pre_filter = fi.dcblocker : fi.lowpass(1, 1720.0);
    
    distortion = *(ba.db2linear(fuzz/10)) : +(0.5) : max(0.0) : min(1.0) 
    : fi.highpass(1, 220) : +(0.5)
    <: _,_ : * : -(0.4) : *(ba.db2linear(fuzz/5)) : max(-1.0) : min(1.0) : +(0.15);

      
    filter = fi.high_shelf(tone + 7.5, 720.0);
    
    stomp = pre_filter : *(10.0) : distortion : filter :
    *(ba.db2linear(volume * 50.0 ) / 100.0) :
    /(20.0);
    
    output = _ : stomp : _;
    
};


