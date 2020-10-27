#!/bin/bash
make server && gdb bin/server -ex run
#cmake .
#make server
#./update_arduino.sh
#bin/server -a lan -p 8083 "$@"
#(cd frontend && node index.js lan)
#(cd frontend && node index.js "$@")
