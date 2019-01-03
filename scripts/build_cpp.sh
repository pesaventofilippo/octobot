#!/bin/bash
cd ../raspberry/
g++ $(pkg-config --libs --cflags opencv) -std=c++11 -g -lwiringPi -o main main.cpp
