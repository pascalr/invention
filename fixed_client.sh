#!/bin/bash
socat PTY,raw,echo=0,link=/tmp/heda1,b115200 tcp:192.168.0.19:8889
