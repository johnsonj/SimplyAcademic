#!/bin/bash

g++ -g -c -Wall -fPIC hello.cpp -o hello.o
g++ -shared hello.o -o hello.so