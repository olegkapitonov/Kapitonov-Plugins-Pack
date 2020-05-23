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
 * This plugin is an octaver pedal emulator.
 * Model of analog octaver.
 *
 * Process:
 *
 * Extract 1-st harmonics,
 * convert to squire form,
 * divide it's frequency by 2 and by 4,
 * modulate input with this signals.
 *
 * Creates 2 additional tones - 1 and 2 octaves below.
 */

declare name "kpp_octaver";
declare author "Oleg Kapitonov";
declare license "GPLv3";
declare version "1.2";

import("stdfaust.lib");

process = output with {

    // Bypass button, 0 - pedal on, 1 -pedal off (bypass on)
    bypass = checkbox("99_bypass");

    level_d1 = ba.db2linear(-20 + vslider("octave1",0,0,30,0.01));
    level_d2 = ba.db2linear(-20 + vslider("octave2",0,0,30,0.01));
    level_dry = ba.db2linear(-30 + vslider("dry",30,0,30,0.01));
    cutoff_freq = vslider("cutoff frequency",160,100,200,0.1);

    // Extract 1-st harmonics
    pre_filter = fi.dcblocker : fi.lowpass(3, 80) : fi.peak_eq(30, 100, 80) :
      fi.peak_eq(20, 440, 200);

    // Convert to squire form (Shmitt trigger)
    distortion = (+ : co.compressor_mono(100, -80, 0.1, 0.1) :
      ma.signum : max(-0.0000001) : min(0.0000001)) ~ _;
    octaver = distortion : fi.zero(1) : max(0.0) : ma.signum : *(-2.0) : +(1.0) :
      (* : +(0.1) : *(10000.0) : max(-1.0) : min(1.0)) ~ _ :
      max(0.0) : min(1.0);

    // Divide 1-st harmonics by 2 - 1 octave below
    down1 = _ <: fi.highpass(1,260) ,(pre_filter : octaver) : * :
      fi.lowpass(3, cutoff_freq) : fi.highpass(3, 40) : fi.highpass(1, 80);

    // Divide by 4 - 2 octaves below
    down2 = _ <: fi.highpass(5,240) ,(pre_filter : octaver : -(0.5) : octaver) : * :
      fi.lowpass(3, cutoff_freq / 2.0);

    // Modulate input signal
    stomp =  _ <: down1,down2 : *(level_d1),*(level_d2) :
      + : *(2.0) : fi.dcblocker;

    output = _,_ : + <: *(level_dry),ba.bypass1(bypass, stomp) : + <: _,_;

};


