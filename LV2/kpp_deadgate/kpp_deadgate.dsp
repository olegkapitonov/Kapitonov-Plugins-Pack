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
 * This pugin is effective noise gate. 
 * 
 * Process chain:
 * 
 * input->deadzone->multigate->output
 * 
 * deadzone - INSTANTLY eliminates the signal below the threshold level.
 *            This kills noise in pauses but distorts the signal around zero level.
 * 
 * multigate - 7-band noise gate. The input signal is divided into 7 frequency bands.
 *             In each band, the signal is eliminated when falling below the
 *             threshold level with attack time 10 ms, hold time 100 ms, release
 *             time 20 ms.
 * 
*/

declare name "kpp_deadgate";
declare author "Oleg Kapitonov";
declare license "GPLv3";
declare version "1.1";

import("stdfaust.lib"); 

process = output with {
  
  deadzone_knob = ba.db2linear(vslider("Dead Zone", -120, -120, 0, 0.001));
  noizegate_knob = vslider("Noise Gate", -120, -120, 0, 0.001);
  
  deadzone = _ <: (max(deadzone_knob) : -(deadzone_knob)),
    (min(-deadzone_knob) : +(deadzone_knob)) : + ;
    
  multigate = _ : fi.filterbank(3, (65, 150, 300, 600, 1200, 2400)) : 
    ef.gate_mono(noizegate_knob, 0.01, 0.02, 0.02),
    ef.gate_mono(noizegate_knob, 0.01, 0.02, 0.02),
    ef.gate_mono(noizegate_knob, 0.01, 0.02, 0.02),
    ef.gate_mono(noizegate_knob, 0.01, 0.02, 0.02),
    ef.gate_mono(noizegate_knob, 0.01, 0.02, 0.02),
    ef.gate_mono(noizegate_knob, 0.01, 0.02, 0.02),
    ef.gate_mono(noizegate_knob, 0.01, 0.02, 0.02) :> _;
    
    output = _,_ :> fi.highpass(1,10) : deadzone : multigate :
    *(ba.db2linear(-6.0)) <: _,_ ;
};
 
 
