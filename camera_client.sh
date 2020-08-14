#!/bin/bash
socat /dev/video0,raw,echo=0,b115200 tcp-listen:8889,reuseaddr
