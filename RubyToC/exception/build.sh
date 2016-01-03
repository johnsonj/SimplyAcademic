#!/bin/bash

g++ -g -c -Wall -std=c++11 -fPIC exception.cpp -o exception.o
g++ -shared exception.o -o exception.so