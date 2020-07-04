#include <iostream>
#include <algorithm>
#include <vector>
#include <zbar.h>

#include <opencv2/core.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/highgui.hpp>
#include <string>

#include "programme/common.h"

#include "programme/setup.h"

#include "io_common.h"
#include <chrono>
#include <thread>

#include "vision/hr_code.hpp"

using namespace std;
using namespace cv;
using namespace zbar;

typedef struct {
  string type;
  string data;
  vector <Point> location;
} DecodedObject;

#define MAX_X AXIS_X_MAX_POS
#define MAX_Z AXIS_Z_MAX_POS

// Sends a command to arduino to move, then analyze in real time.
// When a new sticker appears, it asks the arduino what position it is.
// When a sticker disappers, it asks the arduino what position it is.
// It can then triangulate the real position of the 

class MovingDetectedCodes {
  public:
    double firstKnownPosition;
    double lastKnownPosition;
};

void waitForMessageDone() {
  while (!linuxInputAvailable()) {
    Mat frame;
    captureVideoImage(frame);
    this_thread::sleep_for(chrono::milliseconds(200));
    vector<HRCode> codes = detectHRCodes(frame);
    for (auto it = codes.begin(); it != codes.end(); it++) {
      HRCode code = *it;
      cerr << "FOUND: " << code << endl;
    }
  }

  string str;
  cin >> str;
  if (str != MESSAGE_DONE) {
    cerr << "not done yet, waiting for message done" << endl;
    waitForMessageDone();
  } else {
    cerr << "OK received message done\n";
  }
}

void move(const char* txt, double pos) {
  cout << txt << pos << endl;
  waitForMessageDone();
  this_thread::sleep_for(chrono::milliseconds(50));
}

// x and z position is the position of the camera.
int main(int argc, char *argv[])
{
  signal(SIGINT, signalHandler);

  double heights[] = {0.0};
  //double xSweepIntervals[] = {0, 100, 200, 300, 400, 500, 600, 700, '\0'};
  double zStep = MAX_Z / 1;
  double xStep = MAX_X / 4;

  bool xUp = false; // Wheter the x axis goes from 0 to MAX or from MAX to 0

  double x = MAX_X;
  double z = 0;

  for (int i = 0; i < (sizeof(heights) / sizeof(double)); i++) {
    move("MZ",0);
    bool zUp = true; // Wheter the z axis goes from 0 to MAX or from MAX to 0

    move("MY",heights[i]);
    for (x = xUp ? 0 : MAX_X; xUp ? x <= MAX_X : x >= 0; x += xStep * (xUp ? 1 : -1)) {
      move("MX",x);
      for (z = zUp ? 0 : MAX_Z; zUp ? z <= MAX_Z : z >= 0; z += zStep * (zUp ? 1 : -1)) {
        move("MZ",z);
        // Detect new ones and move to them to get exact position.
        // findBarCodes();
        
      }
      zUp = !zUp;
    }
    xUp = !xUp;
  }

  return 0;
}


