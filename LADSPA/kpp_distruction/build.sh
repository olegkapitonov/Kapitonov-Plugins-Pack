#!/bin/bash

g++ -O2 -fPIC -shared -Dmydsp=kpp_distruction -I/usr/local/share/faust faust-generated/kpp_distruction.dsp.cpp -o kpp_distruction.so

