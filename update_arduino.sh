#!/bin/bash
rm -R ../Arduino/libraries/* || true
mkdir ../Arduino/libraries/core
mkdir ../Arduino/libraries/core/reader
mkdir ../Arduino/libraries/core/writer
mkdir ../Arduino/libraries/utils
mkdir ../Arduino/libraries/lib
cp src/utils/utils.h ../Arduino/libraries/utils/utils.h
cp src/utils/utils.cpp ../Arduino/libraries/utils/utils.cpp

cp src/core/writer/writer.h ../Arduino/libraries/core/writer/writer.h
cp src/core/writer/writer.h ../Arduino/libraries/core/writer.h
cp src/core/writer/writer.cpp ../Arduino/libraries/core/writer/writer.cpp
cp src/core/writer/writer.cpp ../Arduino/libraries/core/writer.cpp

cp src/utils/constants.h ../Arduino/libraries/utils/constants.h
cp src/core/program.h ../Arduino/libraries/core/program.h
cp src/core/referencer.h ../Arduino/libraries/core/referencer.h
cp src/core/program.cpp ../Arduino/libraries/core/program.cpp
cp src/core/input_parser.h ../Arduino/libraries/core/input_parser.h
cp src/core/input_parser.cpp ../Arduino/libraries/core/input_parser.cpp
cp src/core/serialize.h ../Arduino/libraries/core/serialize.h
cp src/core/Motor.h ../Arduino/libraries/core/Motor.h
cp src/core/StepperMotor.h ../Arduino/libraries/core/StepperMotor.h
cp src/core/DCMotor.h ../Arduino/libraries/core/DCMotor.h
cp src/core/Encoder.h ../Arduino/libraries/core/Encoder.h
cp src/lib/ArduinoMock.h ../Arduino/libraries/lib/ArduinoMock.h
cp src/core/reader/reader.h ../Arduino/libraries/core/reader/reader.h
