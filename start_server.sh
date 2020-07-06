#!/bin/bash
cmake .
make
(cd frontend && node index.js lan)
#(cd frontend && node index.js "$@")
