#!/bin/bash
(cd client && rails server -p 3001 -d)
make daemon && bin/daemon
