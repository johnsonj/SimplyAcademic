#!/bin/bash

g++ -g -c -Wall -fPIC resource.cpp -o resource.o
g++ -shared resource.o -o resource.so