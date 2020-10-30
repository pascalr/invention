#!/bin/bash

var="$(bin/which_acm0)"

if [[ $var =~ "fixed" ]]; then
  socat /dev/ttyACM0,raw,echo=0,b115200 tcp-listen:8889,reuseaddr
else
  socat /dev/ttyACM1,raw,echo=0,b115200 tcp-listen:8889,reuseaddr
fi
