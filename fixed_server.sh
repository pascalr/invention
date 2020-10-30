#!/bin/bash

if [ $# -eq 0 ]
  then
    echo "Error. You must specify which number of ACM to use. Run bin/which_acm0 first to know that."
    exit -1
fi

socat /dev/ttyACM$1,raw,echo=0,b115200 tcp-listen:8889,reuseaddr
