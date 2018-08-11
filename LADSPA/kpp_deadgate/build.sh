#!/bin/bash

g++ -O2 -fPIC -shared -Dmydsp=kpp_deadgate -I/usr/local/share/faust faust-generated/kpp_deadgate.dsp.cpp -o kpp_deadgate.so

