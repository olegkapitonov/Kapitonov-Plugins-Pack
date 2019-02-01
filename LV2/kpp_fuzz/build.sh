#!/bin/bash

mkdir faust-temp

cp kpp_fuzz.dsp faust-temp

cd faust-temp

faust2lv2 -keep kpp_fuzz.dsp

cp kpp_fuzz/kpp_fuzz.cpp ../faust-generated

cd ..

rm -rf faust-temp

g++ -shared -O2 -Wl,--no-as-needed '-DDLLEXT=".so"' -fPIC -I. '-DPLUGIN_URI="https://faustlv2.bitbucket.io/kpp_fuzz"' -DFAUST_META=1 -DFAUST_MIDICC=1 -DFAUST_MTS=1 -DFAUST_UI=0 -DVOICE_CTRLS=1  faust-generated/kpp_fuzz.cpp -o kpp_fuzz.lv2/kpp_fuzz.so

cd kpp_fuzz_ui.src

g++ -shared -O2 -Wl,--no-as-needed -fPIC `pkg-config --cflags --libs cairo` -L/usr/X11/lib -lX11 -lm -DPLUGIN_URI=\"https://faustlv2.bitbucket.io/kpp_fuzz\" -DFAUST_META=1 -DFAUST_MIDICC=1 -DFAUST_MTS=1 -DFAUST_UI=1 -DVOICE_CTRLS=1 kpp_fuzz_gui.c -o kpp_fuzzui.so

cd ..

cp kpp_fuzz_ui.src/kpp_fuzzui.so kpp_fuzz.lv2
