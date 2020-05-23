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
 * This pugin is very primitive imitator of humbucker sound (Les Paul)
 * for single-coiled guitars (Stratocaster).
 * Good for playing hard rock and metal with Stratocaster.
 * Probably bad for playing blues :))
 *
*/

declare name "kpp_single2humbucker";
declare author "Oleg Kapitonov";
declare license "GPLv3";
declare version "1.2";

import("stdfaust.lib");

delay_samples = ma.SR / 2880 / 2;


process = output with {

  effect_knob = vslider("Humbuckerize", 1, 0, 1, 0.001);
  filter_knob = vslider("Bass Cut", 20, 20, 720, 0.001);

  effect = fi.highpass(1,20)
    <: _, de.delay(50, delay_samples) :
    + : fi.lowpass(2, 5500) : fi.peak_eq(6.0, 550, 750);

  filter = fi.highpass(1,filter_knob);


  output = _,_ :> _ <: (*(effect_knob) : effect), (*(1.0 - effect_knob)) : + :
  filter : *(ba.db2linear(-10.0)) <: _,_ ;
};


