#ifndef JAR_PARSER_H
#define JAR_PARSER_H

#include <Eigen/Dense>
 
using namespace std;
using namespace Eigen;

class DetectedHRCodeAbsolutePosition {
  public:
    DetectedHRCodeAbsolutePosition(HRCodePosition pos, double x0, double y0, double z0) : position(pos), x(x0), y(y0), z(z0) {
    }
    HRCodePosition position;
    double x;
    double y;
    double z;
};

double calculateJarHeight(double perceivedWidth) {
  double distanceFromCam = HRCODE_WIDTH * CAMERA_FOCAL_POINT / perceivedWidth;
  return DISTANCE_CAMERA_SHELF - distanceFromCam;
}

void convertToAbsolutePosition(DetectedHrCodePosition& pos, DetectedHrCodeAbsolutePosition& result) {
  Transform t(AngleAxis(angle,axis));
  Vector3d v(1,2,3);

  double camDeltaX = (RAYON - CAMERA_OFFSET) * cos(pos.angle / 180 * PI);
  double camDeltaZ = (RAYON - CAMERA_OFFSET) * sin(pos.angle / 180 * PI);
  double camX = toolX - camDeltaX;
  double camZ = toolZ - camDeltaZ;

  // ATTENTION: Je dois transférer les coordonées x,y et données x,z; Je dois tourner...

  for (auto pos = positions.begin(); pos != positions.end(); ++pos) {
    double pixelsPerMM = pos->scale;
    double imgDeltaX = (pos->x - CAMERA_WIDTH / 2.0) * scale;
    double imgDeltaY = (pos->y - CAMERA_HEIGHT / 2.0) * scale;

    // x is side by side error, y is forward and backward error.
    //pos->y;
  }

}

#endif
