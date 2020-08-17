#ifndef JAR_POSITION_DETECTOR
#define JAR_POSITION_DETECTOR

// Maybe rename to pinpoint

#include "position.h"
#include "sweep.h"
#include <vector>
#include "model.h"
 
using namespace std;
using namespace Eigen;

/*double calculateJarHeight(double perceivedWidth) {
  double distanceFromCam = HR_CODE_WIDTH * CAMERA_FOCAL_POINT / perceivedWidth;
  return DISTANCE_CAMERA_SHELF - distanceFromCam;
}

Vector2d cameraPosition(Vector2d toolPosition, double angle) {

  double offset = CAMERA_TOOL_DISTANCE;
  Vector2d cameraOffset(offset*cosd(angle),offset*sind(angle));
  return toolPosition - cameraOffset;
}

Vector2d jarOffset(Vector2d imgCenter, double angle, double scale) {

  Vector2d imgCenterOffset = imgCenter - Vector2d(CAMERA_WIDTH/2, CAMERA_HEIGHT/2) ;

  // Gauche droite sur l'image (donc jarCenter.x) c'est les z quand l'angle est à 0.
  // jarCenter.x positif c'est z négatif.
  // jarCenter.y positif c'est x négatif.
  Vector2d jarCenter;
  jarCenter << -imgCenterOffset(1), -imgCenterOffset(0);

  jarCenter /= scale; // Translate pixels dimensions into mm.

  // I must rotate the offset value because the camera is rotated.
  Transform<double, 2, TransformTraits::Affine> t(Rotation2D<double>(angle / 180.0 * PI));
  jarCenter = t * jarCenter;

  return jarCenter;
}


Vector2d convertToAbsolutePosition(Vector2d toolPosition, double angle, Vector2d imgCenter, double scale) {
  
  Vector2d cameraPos = cameraPosition(toolPosition, angle);

  return cameraPos + jarOffset(imgCenter, angle, scale);
}*/

void pinpointCode(Heda& heda, DetectedHRCode& input) {

  /*Vector2d toolPosition;
  toolPosition << input.x, input.z;

  Vector2d jarCenter;
  jarCenter << input.code.x, input.code.y;

  Vector2d jarPosition = convertToAbsolutePosition(toolPosition, input.angle, jarCenter, input.code.scale);

  PositionedHRCode result(input.code, jarPosition(0), 0, jarPosition(1));
  return result;*/
}

/*void pinpointCode(Heda& heda, vector<DetectedHRCode>& input) {
  for (auto it = input.begin(); it != input.end(); ++it) {
    pinpoint(heda, *it);
  }
}*/

#endif
