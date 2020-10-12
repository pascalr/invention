#!/bin/bash
if test -f "tmp/server.pid"; then
  kill `cat tmp/server.pid`
  rm "tmp/server.pid"
fi
if test -f "client/tmp/pids/server.pid"; then
  kill `cat client/tmp/pids/server.pid`
fi
