#!/bin/bash
ffmpeg -i udp://localhost:1234 -vcodec copy output.mp4
ffmpeg -f v4l2 -framerate 25 -video_size 640x480 -i /dev/video0 output.mkvA udp://10.1.0.102:1234
socat PTY,raw,echo=0,link=/tmp/hedavideo0,b115200 tcp:192.168.0.19:8889
