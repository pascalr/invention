#!/bin/bash
g++ -g test.cpp -DDEBUG -x gdb.txt && clear && gdb a.out
