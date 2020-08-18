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
#include "../utils/constants.h"
#include "fake_program.h"

#include <chrono>
#include <thread>

#include "position.h"
#include "model.h"
#include "heda.h"

using namespace std;
using namespace cv;

#define MAX_X AXIS_X_MAX_POS
#define MAX_Z AXIS_Z_MAX_POS

// Sends a command to arduino to move, then analyze in real time.
// When a new sticker appears, it asks the arduino what position it is.
// When a sticker disappers, it asks the arduino what position it is.
// It can then triangulate the real position of the 

void detectCode(Heda& heda, Mat& frame, PolarCoord c) {

  cout << "Running detect code." << endl;
  HRCodeParser parser(0.2, 0.2);
  vector<HRCode> positions;
  parser.findHRCodes(frame, positions, 100);

  if (!positions.empty()) {
    for (auto it = positions.begin(); it != positions.end(); ++it) {
      cout << "Detected one HRCode!!!" << endl;
      DetectedHRCode d(*it, c);
      heda.db.insert(heda.codes, d);
    }
    return;
  }
  cout << "No codes were detected..." << endl;
}

void sweepCallback(Heda& heda) {
  cout << "sweepCallback(Heda& heda)\n";
  BOOST_LOG_TRIVIAL(debug) << "Capturing frame.";
  Mat frame;
  heda.captureFrame(frame);

  BOOST_LOG_TRIVIAL(debug) << "Trying to detect HR code positions.";
  detectCode(heda, frame, heda.getPosition()); 
}

void calculateSweepMovements(Heda& heda, vector<Movement>& movements) {
  
  cout << "calculateSweepMovements\n";

  double heights[] = {0.0};
  double zStep = MAX_Z / 4;
  double xStep = MAX_X / 4;

  bool xUp = false; // Wheter the x axis goes from 0 to MAX or from MAX to 0

  double x = MAX_X;
  double z = 0.0;

  PolarCoord oldP = heda.getPosition();

  for (unsigned int i = 0; i < (sizeof(heights) / sizeof(double)); i++) {
    bool zUp = true; // Wheter the z axis goes from 0 to MAX or from MAX to 0
    for (x = xUp ? 0.0 : MAX_X; xUp ? x <= MAX_X : x >= 0.0; x += xStep * (xUp ? 1 : -1)) {
      for (z = zUp ? 0.0 : MAX_Z; zUp ? z <= MAX_Z : z >= 0.0; z += zStep * (zUp ? 1 : -1)) {
        //UserCoord c(c << x, heights[i], z);
        //PolarCoord p = toolCartesianToPolar(c);
        //calculateGoto(movements, oldP, p, [&heda](){sweepCallback(heda);});
        //oldP = p;
      }
      zUp = !zUp;
    }
    xUp = !xUp;
  }
}
