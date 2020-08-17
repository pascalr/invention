#ifndef SWEEP_H
#define SWEEP_H

#include <iostream>
#include <algorithm>
#include <vector>

#include <string>

#include "../utils/io_common.h"
#include "../lib/lib.h"
#include "../lib/hr_code.h"
#include "../lib/serial.h"
#include "../lib/opencv.h"
#include "../helper/logging.h"
#include "../config/constants.h"
#include "fake_program.h"

#include <chrono>
#include <thread>

#include "position.h"
#include "model.h"
#include "heda.h"

class Heda;
//class Mat;

void detectCode(Heda& heda, Mat& frame, PolarCoord c);

void calculateSweepMovements(Heda& heda, std::vector<Movement>& movements);

#endif
