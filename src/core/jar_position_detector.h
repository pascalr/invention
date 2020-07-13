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
  PositionedHRCode convert(DetectedHRCode input) {


  }

  void run(vector<DetectedHRCode> input, vector<PositionedHRCode> result) {
    
  }
};

#endif
