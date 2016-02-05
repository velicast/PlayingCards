#!/bin/bash
rm e
clear
g++ c.cpp process.cpp segment.cpp characterize.cpp classify.cpp -o e -I./ `pkg-config --cflags --libs opencv`
./e < in
