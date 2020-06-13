#!/bin/bash
# -std=c++11 -I/usr/include/python2.7 -lpython2.7 => This is for matplotlib
g++ test.cpp -DDEBUG -std=c++11 -I/usr/include/python2.7 -lpython2.7 && clear && ./a.out 2> /dev/null
