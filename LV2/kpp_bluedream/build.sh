#!/bin/bash

mkdir faust-temp

cp kpp_bluedream.dsp faust-temp

cd faust-temp

faust2lv2 -keep kpp_bluedream.dsp

cp kpp_bluedream/kpp_bluedream.cpp ../faust-generated

cd ..

rm -rf faust-temp

g++ -shared -O2 -Wl,--no-as-needed '-DDLLEXT=".so"' -fPIC -I. '-DPLUGIN_URI="https://faustlv2.bitbucket.io/kpp_bluedream"' -DFAUST_META=1 -DFAUST_MIDICC=1 -DFAUST_MTS=1 -DFAUST_UI=0 -DVOICE_CTRLS=1  faust-generated/kpp_bluedream.cpp -o kpp_bluedream.lv2/kpp_bluedream.so

cd kpp_bluedream_ui.src

g++ -shared -O2 -Wl,--no-as-needed -fPIC `pkg-config --cflags --libs cairo` -L/usr/X11/lib -lX11 -lm -DPLUGIN_URI=\"https://faustlv2.bitbucket.io/kpp_bluedream\" -DFAUST_META=1 -DFAUST_MIDICC=1 -DFAUST_MTS=1 -DFAUST_UI=1 -DVOICE_CTRLS=1 kpp_bluedream_gui.c -o kpp_bluedreamui.so

cd ..

cp kpp_bluedream_ui.src/kpp_bluedreamui.so kpp_bluedream.lv2
