#!/bin/bash

swig -c++ -python traveller.i

gcc -c -O2 -std=c++11 -fPIC DPoint.cpp CityMap.cpp traveller_wrap.cxx -I/usr/local/anaconda2/include/python2.7

# ld -shared DPoint.o CityMap.o traveller_wrap.o -o _traveller.so 

g++ -shared DPoint.o CityMap.o traveller_wrap.o -lc -lstdc++ -pthread  -o _ga_traveller.so

# gcc -std=c++11 -fPIC -lm -lstdc++ -pthread DPoint.cpp CityMap.cpp test.cpp -o test -g
