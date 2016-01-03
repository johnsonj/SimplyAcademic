#!/bin/bash

clang++  -std=c++14 transaction.cpp transaction_test.cpp -o transaction
./transaction

clang++  -std=c++14 lazy_test.cpp -o lazy
./lazy