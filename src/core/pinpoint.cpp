#include "pinpoint.h"
#include "position.h"
#include "heda.h"
#include <vector>
#include "model.h"

#include <Eigen/Core>
#include <Eigen/Geometry>
using namespace Eigen;

 
using namespace std;

// https://www.pyimagesearch.com/2015/01/19/find-distance-camera-objectmarker-using-python-opencv/
// F = (P x  D) / W
// The focal point (F) equals the apparent width in pixels (P) times the distance from the camera D
// divided by the actual width of the object (W)
// I find F by an average with some data.
// Then I can always get the distance from the other values.
// D’ = (W x F) / P

double heightOffset(Heda& heda, DetectedHRCode& input) {

  // FIXME: Camera offset of a few mm from the bottom of the arm.

  // F = 3.35mm
  // P = pixelsPerMM * WIDTH
  // W = WIDTH
  // Distance D = ???
  // D = WIDTH X 3.35mm / (pixelsPerMM * WIDTH)
  //double pixelsPerMM = input.scale;
  return heda.config.camera_focal_point / input.scale;
  //double perseivedWidth = HR_CODE_WITH * input.scale;
  //double distanceFromCam = HR_CODE_WIDTH * heda.config.camera_focal_point / perceivedWidth;
  //return heda.config.camera_focal_point / input.scale;
}

Vector2d imageOffset(Heda& heda, DetectedHRCode& input) {

  // Using heda config to get camera width because the image is not the full image, it is only the cropped of the code.
  double width = heda.config.camera_width;
  double height = heda.config.camera_height;

  Vector2d jarCenter; jarCenter << input.centerX, input.centerY;
  Vector2d imgCenterOffset = jarCenter - Vector2d(width/2, height/2) ;

  // Gauche droite sur l'image (donc jarCenter.x) c'est les z quand l'angle est à 0.
  // jarCenter.x positif c'est z négatif.
  // jarCenter.y positif c'est x négatif.
  Vector2d jarOffset; jarOffset << -imgCenterOffset(1), -imgCenterOffset(0);

  jarOffset /= input.scale; // Translate pixels dimensions into mm.

  // I must rotate the offset value because the camera is rotated.
  Transform<double, 2, TransformTraits::Affine> t(Rotation2D<double>(input.t / 180.0 * PI));
  jarOffset = t * jarOffset;

  return jarOffset;
}

void pinpointCode(Heda& heda, DetectedHRCode& input) {

  UserCoord camPos = heda.toUserCoord(PolarCoord(input.h, input.v, input.t), heda.config.camera_radius);
  double heightOffset0 = heightOffset(heda, input);

  Vector2d imgOffset = imageOffset(heda, input);

  input.lid_x = camPos.x + imgOffset(0);
  input.lid_y = camPos.y - heightOffset0;
  input.lid_z = camPos.z + imgOffset(1);
}
