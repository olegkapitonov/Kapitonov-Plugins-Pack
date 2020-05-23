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
 * This plugin is the universal tube amplifier emulator
 * with tonestack, gain and volume(mastergain) knobs.
 *
 * Process chain:
 *
 * IN->preamp_convolver->*drive_knob->preamp->tonestack->
 *    <-----------------------------<----------------<---
 *    -->*volume_knob->power_amp->cabsym_convolver->OUT
 *
 * Distortion and tonestack parameters loaded
 * from *.tapf profile file.
 * Convolvers work outside this FAUST module.
 * Cabsym convolver may be bypassed.
 */

declare name "kpp_tubeamp";
declare author "Oleg Kapitonov";
declare license "GPLv3";
declare version "1.2";

import("stdfaust.lib");

process = preamp_amp with {

    // Link parameters from *.tapf profile file
    // and knob values with FAUST code.
    drive = fvariable(float DRIVE_CTRL, <math.h>);
    volume = fvariable(float VOLUME_CTRL, <math.h>);
    mastergain = fvariable(float MASTERGAIN_CTRL, <math.h>);

    // Bias signal before distortion
    amp_bias = fvariable(float AMP_BIAS_CTRL, <math.h>);
    // Threshold of distortion
    amp_Upor = fvariable(float AMP_UPOR_CTRL, <math.h>);
    // Severity/softness of distortion
    amp_Kreg = fvariable(float AMP_KREG_CTRL, <math.h>);

    // The same parameters for preamp
    preamp_bias = fvariable(float PREAMP_BIAS_CTRL, <math.h>);
    preamp_Upor = fvariable(float PREAMP_UPOR_CTRL, <math.h>);
    preamp_Kreg = fvariable(float PREAMP_KREG_CTRL, <math.h>);


    tonestack_low = fvariable(float LOW_CTRL, <math.h>);
    tonestack_middle = fvariable(float MIDDLE_CTRL, <math.h>);
    tonestack_high = fvariable(float HIGH_CTRL, <math.h>);

    tonestack_low_freq = fvariable(float LOW_FREQ_CTRL, <math.h>);
    tonestack_middle_freq = fvariable(float MIDDLE_FREQ_CTRL, <math.h>);
    tonestack_high_freq = fvariable(float HIGH_FREQ_CTRL, <math.h>);

    tonestack_low_band = fvariable(float LOW_BAND_CTRL, <math.h>);
    tonestack_middle_band = fvariable(float MIDDLE_BAND_CTRL, <math.h>);
    tonestack_high_band = fvariable(float HIGH_BAND_CTRL, <math.h>);

    // Gain before preamp
    preamp_level = fvariable(float PREAMP_LEVEL, <math.h>);
    // Gain before amp
    amp_level = fvariable(float AMP_LEVEL, <math.h>);

    // Voltage Sag parameters
    sag_time = fvariable(float SAG_TIME, <math.h>);
    sag_coeff = fvariable(float SAG_COEFF, <math.h>);

    // Output gain
    output_level = fvariable(float OUTPUT_LEVEL, <math.h>);

    // Model of tube nonlinear distortion
    tube(Kreg,Upor,bias,cut) = main : +(bias) : max(cut) with {
        Ks(x) = 1/(max((x-Upor)*(Kreg),0)+1);
        Ksplus(x) = Upor - x*Upor;
        main(Uin) = (Uin * Ks(Uin) + Ksplus(Ks(Uin)));
    };

    // Preamp - has 1 class A tube distortion (non symmetric)
    stage_preamp = fi.lowpass(1,11000) :
    tube(preamp_Kreg,preamp_Upor,preamp_bias,-preamp_Upor);

    stage_tonestack = fi.peak_eq(tonestack_low,tonestack_low_freq,tonestack_low_band) :
    fi.peak_eq(tonestack_middle,tonestack_middle_freq,tonestack_middle_band) :
    fi.peak_eq(tonestack_high,tonestack_high_freq,tonestack_high_band): fi.lowpass(1,11000);

    // Power Amp - has 1 class B tube distortion (symmetric)
    stage_amp = _<: _,*(-1.0) :
    tube(amp_Kreg,amp_Upor,amp_bias,0),
    tube(amp_Kreg,amp_Upor,amp_bias,0) :
    - :
    fi.lowpass(1, 11000);

    // Part of the chain before Voltage Sag in power amp
    pre_sag = _,_ : + : fi.dcblocker : *(ba.db2linear(drive * 0.4) - 1) :
    *(preamp_level) : stage_preamp : fi.dcblocker :*(amp_level) :
    *(ba.db2linear(mastergain * 0.4) - 1) : stage_tonestack;

    // All chain, pre-sag + power amp with Voltage Sag
    preamp_amp = pre_sag :
    (_,_ : (_<: (1.0/_),_),_ : _,* : _,stage_amp : *)
    ~ (_ <: _,_: * : fi.lowpass(1,sag_time) : *(sag_coeff) :
    max(1.0) : min(2.5)) : *(volume) :
    *(output_level) : fi.dcblocker <: _,_;
};


