#!/bin/bash

./stop.sh
(cd client && rails server -p 3001 -d)
make daemon && bin/daemon
