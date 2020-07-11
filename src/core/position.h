#ifndef POSITION_H
#define POSITION_H

#include <Eigen/Dense>

using namespace Eigen;
using namespace std;

Vector2d cameraPosition(Vector2d toolPosition, double angle) {
  Transform t(rot2(angle / 180 * PI));
  //Transform t;
  //t = Rotation2D<float> rot2(angle / 180 * PI);
  double camDeltaX = (RAYON - CAMERA_OFFSET) * cos(pos.angle / 180 * PI);
  double camDeltaZ = (RAYON - CAMERA_OFFSET) * sin(pos.angle / 180 * PI);
  double camX = toolX - camDeltaX;
  double camZ = toolZ - camDeltaZ;
  Vector2d cameraToTool = ();
  Vector2d cameraPos(CAMERA_OFFSET*sin(pos.angle / 180 * PI),CAMERA_OFFSET*cos);

  Vector2d result = toolPosition - ;

  Vector3d v(1,2,3);
}

#endif
