#!/bin/bash
make heda && gdb --args bin/heda "$@"
