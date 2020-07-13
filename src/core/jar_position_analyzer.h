#ifndef JAR_POSITION_DETECTOR
#define JAR_POSITION_DETECTOR

#include "position.h"
#include "sweep.h"
#include <vector>
 
using namespace std;
using namespace Eigen;

class PositionedHRCode {
  public:
    PositionedHRCode(HRCode cod, double x0, double y0, double z0) : code(cod), x(x0), y(y0), z(z0) {
    }
    HRCode code;
    double x;
    double y;
    double z;
};
ostream &operator<<(std::ostream &os, const PositionedHRCode &c) {
  return os << c.code << " at (" << c.x << ", " << c.y << ", " << c.z << ")";
}

double calculateJarHeight(double perceivedWidth) {
  double distanceFromCam = HR_CODE_WIDTH * CAMERA_FOCAL_POINT / perceivedWidth;
  return DISTANCE_CAMERA_SHELF - distanceFromCam;
}

class Jar {
  Vector3d position;
};

// This is separate from sweep because this does not require the sweep action.
// This is done afterward.
// Step1: detect the position of the jar
// Step2: remove near duplicates
// Step3: parse the text
// Step4: if the text is not readable, store into data/negatives
// and later ask the user to translate
class JarPositionAnalyzer {
public:
  PositionedHRCode convert(DetectedHRCode& input) {

    Vector2d toolPosition;
    toolPosition << input.x, input.z;

    Vector2d jarCenter;
    jarCenter << input.code.x, input.code.y;

    Vector2d jarPosition = convertToAbsolutePosition(toolPosition, input.angle, jarCenter, input.code.scale);

    PositionedHRCode result(input.code, jarPosition(0), 0, jarPosition(1));
    return result;
  }

  void run(vector<DetectedHRCode> input, vector<PositionedHRCode>& result) {
    for (auto it = input.begin(); it != input.end(); ++it) {
      PositionedHRCode p = convert(*it);
      result.push_back(p);
    }
  }
};

#endif
