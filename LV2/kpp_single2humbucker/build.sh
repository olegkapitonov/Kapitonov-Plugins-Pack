#!/bin/bash

mkdir faust-temp

cp kpp_single2humbucker.dsp faust-temp

cd faust-temp

faust2lv2 -keep kpp_single2humbucker.dsp

cp kpp_single2humbucker/kpp_single2humbucker.cpp ../faust-generated

cd ..

rm -rf faust-temp

g++ -shared -O2 -Wl,--no-as-needed '-DDLLEXT=".so"' -fPIC -I. '-DPLUGIN_URI="https://faustlv2.bitbucket.io/kpp_single2humbucker"' -DFAUST_META=1 -DFAUST_MIDICC=1 -DFAUST_MTS=1 -DFAUST_UI=0 -DVOICE_CTRLS=1  faust-generated/kpp_single2humbucker.cpp -o kpp_single2humbucker.lv2/kpp_single2humbucker.so

