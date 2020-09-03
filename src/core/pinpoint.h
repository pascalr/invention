#ifndef JAR_POSITION_DETECTOR
#define JAR_POSITION_DETECTOR

// Maybe rename to pinpoint

#include "position.h"
#include <vector>
#include "model.h"
 
using namespace std;
using namespace Eigen;


// https://www.pyimagesearch.com/2015/01/19/find-distance-camera-objectmarker-using-python-opencv/
// F = (P x  D) / W
// The focal point (F) equals the apparent width in pixels (P) times the distance from the camera D
// divided by the actual width of the object (W)
// I find F by an average with some data.
// Then I can always get the distance from the other values.
// D’ = (W x F) / P

double heightOffset(Heda& heda, DetectedHRCode& input) {

  //double perseivedWidth = HR_CODE_WITH * input.scale;
  //double distanceFromCam = HR_CODE_WIDTH * heda.config.camera_focal_point / perceivedWidth;
  return heda.config.camera_focal_point / input.scale;
}

Vector2d imageOffset(DetectedHRCode& input) {

  Vector2d jarCenter; jarCenter << input.centerX, input.centerY;
  Vector2d imgCenterOffset = jarCenter - Vector2d(CAMERA_WIDTH/2, CAMERA_HEIGHT/2) ;

  // Gauche droite sur l'image (donc jarCenter.x) c'est les z quand l'angle est à 0.
  // jarCenter.x positif c'est z négatif.
  // jarCenter.y positif c'est x négatif.
  Vector2d jarOffset; jarOffset << -imgCenterOffset(1), -imgCenterOffset(0);

  jarOffset /= input.scale; // Translate pixels dimensions into mm.

  // I must rotate the offset value because the camera is rotated.
  Transform<double, 2, TransformTraits::Affine> t(Rotation2D<double>(input.coord.t / 180.0 * PI));
  jarOffset = t * jarOffset;

  return jarOffset;
}

void pinpointCode(Heda& heda, DetectedHRCode& input) {

  UserCoord camPos = heda.toUserCoord(input.coord, heda.config.camera_radius);
  double heightOffset0 = heightOffset(heda, input);
  Vector2d imgOffset = imageOffset(input);

  input.lid_coord = UserCoord(camPos.x + imgOffset(0), camPos.y - heightOffset0, camPos.z + imgOffset(1));
}

#endif
