#!/bin/bash

cd LK_FILES
make
cd ..
g++ main.cpp rand/mersenne.o LK_FILES/FileHelper.o LK_FILES/LKHParser.o -Wall -O3 -o main