#!/bin/bash
if test -f "tmp/pids/server.pid"; then
  kill `cat tmp/pids/server.pid`
fi
