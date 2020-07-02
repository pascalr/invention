#!/bin/bash
# -std=c++11 -I/usr/include/python2.7 -lpython2.7 => This is for matplotlib
g++ communicate.cpp -DDEBUG -std=c++11 -I/usr/include/python2.7 -lpython2.7 && ./a.out 2> /dev/null
