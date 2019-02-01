#!/bin/bash

mkdir faust-temp

cp kpp_octaver.dsp faust-temp

cd faust-temp

faust2lv2 -keep kpp_octaver.dsp

cp kpp_octaver/kpp_octaver.cpp ../faust-generated

cd ..

rm -rf faust-temp

g++ -shared -O2 -Wl,--no-as-needed '-DDLLEXT=".so"' -fPIC -I. '-DPLUGIN_URI="https://faustlv2.bitbucket.io/kpp_octaver"' -DFAUST_META=1 -DFAUST_MIDICC=1 -DFAUST_MTS=1 -DFAUST_UI=0 -DVOICE_CTRLS=1  faust-generated/kpp_octaver.cpp -o kpp_octaver.lv2/kpp_octaver.so

