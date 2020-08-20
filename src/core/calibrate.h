#include <iostream>

#include "heda.h"
#include "writer/serial_writer.h"
#include "reader/serial_reader.h"

using namespace std;

class InvalidGrippedJarFormatException : public exception {};

DetectedHRCode detectOneCode(Heda& heda) {
 
  heda.clearDetectedCodes(); 
  heda.detect();
  if (heda.codes.items.size() < 1) {throw MissingHRCodeException();}
  if (heda.codes.items.size() > 1) {throw TooManyHRCodeException();}

  return heda.codes.items[0];
}

// https://www.pyimagesearch.com/2015/01/19/find-distance-camera-objectmarker-using-python-opencv/
// F = (P x  D) / W
// The focal point (F) equals the apparent width in pixels (P) times the distance from the camera D
// divided by the actual width of the object (W)
// I find F by an average with some data.
// Then I can always get the distance from the other values.
// D’ = (W x F) / P

// The focal point can be used to determine the distance of an object.
double computeFocalPoint(double perceivedWidth, double distanceFromCamera, double actualWidth) {

  return perceivedWidth * distanceFromCamera / actualWidth;
}

void Heda::calibrate() {
  // TODO: Validate that the working shelf is clear
  // TODO: Make sure heda is referenced
  // TODO: Use putdown function.
 
  JarFormat format; 
  if (!jar_formats.get(format, gripped_jar.jar_format_id)) {throw InvalidGrippedJarFormatException();}

  Shelf shelf = getWorkingShelf();

  PolarCoord c = toPolarCoord(UserCoord(0.0, shelf.height + format.height + config.grip_offset, 0.0), config.gripper_radius);

  moveTo(PolarCoord(0.0, c.v, 45.0));
  openJaw();
  moveTo(PolarCoord(0.0, config.detect_height, 45.0));
  moveTo(PolarCoord(175.0, config.detect_height, 75.0));
  DetectedHRCode code = detectOneCode(*this);

  // double jarHeight = gripped_jar.height;
  //putdown(); 
  //DetectedHRCode code = detectOneCode(Heda& heda);
  //heda.camera_radius = ...

  double cameraDistance = config.detect_height - format.height - shelf.height;
  double codePixelsWidth = HR_CODE_WIDTH * code.scale;
  config.camera_focal_point = computeFocalPoint(codePixelsWidth, cameraDistance, HR_CODE_WIDTH);
  db.update(configs, config);
}
