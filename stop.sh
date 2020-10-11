#!/bin/bash
kill `cat tmp/server.pid`
kill `cat client/tmp/pids/server.pid`
