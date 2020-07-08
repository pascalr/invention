#!/bin/bash
cmake .
make
bin/server -a lan -p 8083 "$@"
#(cd frontend && node index.js lan)
#(cd frontend && node index.js "$@")
