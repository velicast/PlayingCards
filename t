#!/bin/bash
rm e
clear
g++ t.cpp characterize.cpp -o e -I./ `pkg-config --cflags --libs opencv`
./e
