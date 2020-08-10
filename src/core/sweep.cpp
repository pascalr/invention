#include "sweep.h"

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
#include "../config/setup.h"
#include "../config/constants.h"
#include "fake_program.h"

#include <chrono>
#include <thread>

#include "position.h"
#include "Model.h"
#include "Heda.h"

using namespace std;
using namespace cv;

#define MAX_X AXIS_X_MAX_POS
#define MAX_Z AXIS_Z_MAX_POS

// Sends a command to arduino to move, then analyze in real time.
// When a new sticker appears, it asks the arduino what position it is.
// When a sticker disappers, it asks the arduino what position it is.
// It can then triangulate the real position of the 

void detect(Heda& heda, Mat& frame, PolarCoord c) {

  HRCodeParser parser(0.2, 0.2);
  vector<HRCode> positions;
  parser.findHRCodes(frame, positions, 100);

  if (!positions.empty()) {
    for (auto it = positions.begin(); it != positions.end(); ++it) {
      DetectedHRCode d(*it, c);
      heda.db.addItem(heda.codes, d);
    }
  }
}

std::function<void()> sweepCallback(Heda& heda) {
  return [&heda]() {
    BOOST_LOG_TRIVIAL(debug) << "Capturing frame.";
    Mat frame;
    heda.captureFrame(frame);

    BOOST_LOG_TRIVIAL(debug) << "Trying to detect HR code positions.";
    detect(heda, frame, heda.getPosition()); 
  };
}

void calculateMovements(Heda& heda, vector<Movement>& movements) {

  double heights[] = {0.0};
  double zStep = MAX_Z / 4;
  double xStep = MAX_X / 4;

  bool xUp = false; // Wheter the x axis goes from 0 to MAX or from MAX to 0

  double x = MAX_X;
  double z = 0.0;

  PolarCoord oldP = heda.getPosition();

  for (unsigned int i = 0; i < (sizeof(heights) / sizeof(double)); i++) {


    double angleDest = (x > X_MIDDLE) ? CHANGE_LEVEL_ANGLE_HIGH : CHANGE_LEVEL_ANGLE_LOW;
    movements.push_back(Movement('T', angleDest));
    bool zUp = true; // Wheter the z axis goes from 0 to MAX or from MAX to 0

    for (x = xUp ? 0.0 : MAX_X; xUp ? x <= MAX_X : x >= 0.0; x += xStep * (xUp ? 1 : -1)) {
      for (z = zUp ? 0.0 : MAX_Z; zUp ? z <= MAX_Z : z >= 0.0; z += zStep * (zUp ? 1 : -1)) {
        CartesianCoord c; c << x, heights[i], z;
        PolarCoord p = toolCartesianToPolar(c);
        calculateGoto(movements, oldP, p, sweepCallback(heda));
        oldP = p;
      }
      zUp = !zUp;
    }
    xUp = !xUp;
  }
}
