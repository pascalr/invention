#include <iostream>
#include <algorithm>
#include <vector>
#include <zbar.h>

#include <opencv2/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include "programme/setup.h"

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

void move(const char* txt, double pos) {
  cout << txt << pos << endl;
}

// x and z position is the position of the camera.
int main(int argc, char *argv[])
{
  double heights[] = {0.0};
  //double xSweepIntervals[] = {0, 100, 200, 300, 400, 500, 600, 700, '\0'};
  double zStep = MAX_Z / 2;
  double xStep = MAX_X / 4;

  bool xUp = true; // Wheter the x axis goes from 0 to MAX or from MAX to 0

  double x = 0;
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


