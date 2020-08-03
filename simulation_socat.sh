#!/bin/bash
socat -d -d pty,raw,echo=0,link=/dev/pty8,group-late=dialout,mode=660 pty,raw,echo=0,link=/dev/pty9,group-late=dialout,mode=660
