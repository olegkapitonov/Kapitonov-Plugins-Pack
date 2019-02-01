#!/bin/bash

faust kpp_tubeamp.dsp >faust-generated/kpp_tubeamp_dsp.cpp

g++ -shared -pthread -O2 -Wall '-DDLLEXT=".so"' -fPIC -Wl,--no-as-needed -lfftw3 -lfftw3f -lm -I. '-DPLUGIN_URI="https://faustlv2.bitbucket.io/kpp_tubeamp"' -DFAUST_META=1 -DFAUST_MIDICC=1 -DFAUST_MTS=1 -DFAUST_UI=0 -DVOICE_CTRLS=1  kpp_tubeamp.cpp -o kpp_tubeamp.lv2/kpp_tubeamp.so

cd kpp_tubeamp_ui.src

g++ -shared -pthread -O2 -Wall -fPIC -Wl,--no-as-needed  `pkg-config --cflags --libs cairo` -L/usr/X11/lib -lX11 -lm -DPLUGIN_URI=\"https://faustlv2.bitbucket.io/kpp_tubeamp\" -DFAUST_META=1 -DFAUST_MIDICC=1 -DFAUST_MTS=1 -DFAUST_UI=1 -DVOICE_CTRLS=1 kpp_tubeamp_gui.c -o kpp_tubeampui.so

cd ..

cp kpp_tubeamp_ui.src/kpp_tubeampui.so kpp_tubeamp.lv2
