#!/bin/bash
rm e
clear
g++ d.cpp process.cpp segment.cpp characterize.cpp -o e `pkg-config --cflags --libs opencv`
./e
