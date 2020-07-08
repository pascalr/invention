#!/bin/bash
rm -R ../Arduino/libraries/*
cp -R src/core ../Arduino/libraries/core
cp -R src/utils ../Arduino/libraries/utils
cp -R src/config ../Arduino/libraries/config
cp -R src/lib ../Arduino/libraries/lib
