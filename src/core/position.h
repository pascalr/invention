#ifndef POSITION_H
#define POSITION_H

// TODO: Only keep Eigen/Core in header file
#include <Eigen/Core>
#include <Eigen/Geometry>

using namespace Eigen;
using namespace std;

Vector2d cameraPosition(Vector2d toolPosition, double angle) {

  double offset = RAYON - CAMERA_OFFSET;
  Vector2d cameraOffset(offset*cos(angle / 180 * PI),offset*sin(angle / 180 * PI));
  return toolPosition - cameraOffset;
}

Vector2d convertToAbsolutePosition(Vector2d toolPosition, double angle, Vector2d jarCenter, double scale) {
  
  Vector2d cameraPos = cameraPosition(toolPosition, angle);

  Vector2d imgDelta = jarCenter - Vector2d(CAMERA_WIDTH/2, CAMERA_HEIGHT/2);
  imgDelta *= scale; // Translate pixels dimensions into mm.

  // I must rotate the offset value because the camera is rotated.
  Transform t(rot2(angle / 180 * PI));
  imgDelta.rotate(t);

  return cameraPos + imgDelta
}

#endif
