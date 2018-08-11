#!/bin/bash

g++ -shared -O2 '-DDLLEXT=".so"' -fPIC -I. '-DPLUGIN_URI="https://faustlv2.bitbucket.io/kpp_distruction"' -DFAUST_META=1 -DFAUST_MIDICC=1 -DFAUST_MTS=1 -DFAUST_UI=0 -DVOICE_CTRLS=1  faust-generated/kpp_distruction.cpp -o kpp_distruction.lv2/kpp_distruction.so

cd kpp_distruction_ui.src

g++ -shared -O2 -fPIC `pkg-config --cflags --libs cairo` -L/usr/X11/lib -lX11 -lm -DPLUGIN_URI=\"https://faustlv2.bitbucket.io/kpp_distruction\" -DFAUST_META=1 -DFAUST_MIDICC=1 -DFAUST_MTS=1 -DFAUST_UI=1 -DVOICE_CTRLS=1 kpp_distruction_gui.c -o kpp_distructionui.so

cd ..

cp kpp_distruction_ui.src/kpp_distructionui.so kpp_distruction.lv2
