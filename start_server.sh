#!/bin/bash
#(cd frontend && node index.js "$@")
make
(cd frontend && node index.js lan)
