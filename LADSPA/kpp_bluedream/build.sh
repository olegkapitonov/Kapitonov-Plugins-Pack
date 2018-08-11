#!/bin/bash

g++ -O2 -fPIC -shared -Dmydsp=kpp_bluedream -I/usr/local/share/faust faust-generated/kpp_bluedream.dsp.cpp -o kpp_bluedream.so

