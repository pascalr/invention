#!/bin/bash
rm -R ../Arduino/libraries/* || true
mkdir ../Arduino/libraries/core
mkdir ../Arduino/libraries/core/reader
mkdir ../Arduino/libraries/utils
mkdir ../Arduino/libraries/config
mkdir ../Arduino/libraries/lib
cp src/core/axis.h ../Arduino/libraries/core/axis.h
cp src/core/axis.cpp ../Arduino/libraries/core/axis.cpp
cp src/utils/utils.h ../Arduino/libraries/utils/utils.h
cp src/utils/utils.cpp ../Arduino/libraries/utils/utils.cpp
cp src/core/writer.h ../Arduino/libraries/core/writer.h
cp src/core/writer.cpp ../Arduino/libraries/core/writer.cpp
cp src/config/constants.h ../Arduino/libraries/config/constants.h
cp src/config/shelves_constants.h ../Arduino/libraries/config/shelves_constants.h
cp src/config/calibration_constants.h ../Arduino/libraries/config/calibration_constants.h
cp src/config/camera_constants.h ../Arduino/libraries/config/camera_constants.h
cp src/config/setup.h ../Arduino/libraries/config/setup.h
cp src/core/program.h ../Arduino/libraries/core/program.h
cp src/core/input_parser.h ../Arduino/libraries/core/input_parser.h
cp src/core/input_parser.cpp ../Arduino/libraries/core/input_parser.cpp
cp src/core/serialize.h ../Arduino/libraries/core/serialize.h
cp src/core/Motor.h ../Arduino/libraries/core/Motor.h
cp src/core/StepperMotor.h ../Arduino/libraries/core/StepperMotor.h
cp src/core/DCMotor.h ../Arduino/libraries/core/DCMotor.h
cp src/core/Encoder.h ../Arduino/libraries/core/Encoder.h
cp src/lib/ArduinoMock.h ../Arduino/libraries/lib/ArduinoMock.h
cp src/core/reader/reader.h ../Arduino/libraries/core/reader/reader.h
