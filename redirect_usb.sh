#!/bin/bash
sudo cat /dev/ttyUSB0 | nc 192.168.0.18 1234

# sudo usermod -a -G dialout pascalr.
# utils:socat /dev/ttyUSB0,raw,echo=0 tcp-listen:8888,reuseaddr
# usb-devices
# dmesg

socat /dev/ttyUSB0,raw,echo=0,b115200 tcp-listen:8888,reuseaddr
socat /dev/ttyUSB0,raw,echo=0 tcp-listen:8888,reuseaddr

# Then on 1st PC you can connect to 2nd PC with socat and provide the data on a pseudo terminal /dev/ttyVUSB0 for your application:

#socat PTY,raw,echo=0,link=/dev/ttyVUSB0 tcp:192.168.0.18:8888
socat PTY,raw,echo=0,link=/dev/ttyUSB0,b115200 tcp:192.168.0.18:8888

#/dev/ttyS0 and /dev/ttyAMA0 are inbuilt UARTS /dev/ttyprintk MAY be yours.
#NOT all USB devices/drivers are "tty".
#Unplug the device, list /dev then plug in and see what has changed.

