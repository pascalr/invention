#!/bin/bash
g++ -g test.cpp -DDEBUG -std=c++11 -I/usr/include/python2.7 -lpython2.7 -x gdb.txt && clear && gdb a.out
