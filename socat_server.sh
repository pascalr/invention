#!/bin/bash
socat /dev/ttyACM0,raw,echo=0,b115200 tcp-listen:8888,reuseaddr
