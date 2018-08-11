#!/bin/bash

g++ -shared -O2 '-DDLLEXT=".so"' -fPIC -I. '-DPLUGIN_URI="https://faustlv2.bitbucket.io/kpp_deadgate"' -DFAUST_META=1 -DFAUST_MIDICC=1 -DFAUST_MTS=1 -DFAUST_UI=0 -DVOICE_CTRLS=1  faust-generated/kpp_deadgate.cpp -o kpp_deadgate.lv2/kpp_deadgate.so

