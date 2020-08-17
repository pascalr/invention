#include <iostream>

#include "core/heda.h"
#include "core/writer/serial_writer.h"
#include "core/reader/serial_reader.h"

using namespace std;

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
  // TODO: Validate that Heda is gripping a reference jar.

  Heda should have a current jar variable.

  putdown(); 
  drop the reference jar
  goto apprimatively on the of the jar with the camera
  DetectedHRCode code = detectOneCode(Heda& heda);

  Vector3d destination;
  //destination << 0.0, heda.config.working_shelf_idworkingShelfHeight + height + 4, 45.0;

  heda.reference(); // Make sure it is referenced.
  //heda.move(Movement('T', CHANGE_LEVEL_ANGLE_LOW));
  //heda.move(Movement('Y', WORKING_SHELF_HEIGHT+height+4));
  //heda.move(Movement('T', 30.0));
  heda.camera_radius = ...
  heda.camera_focal_point = ...

  heda.release();

  cerr << "Please add the reference jar inside the jaws. Press any key when ready..";
  cin >> key;

  destination.y = MAX_WORKING_HEIGHT;
  heda.moveTo(destination);

  DetectedHRCode code = heda.detectOneCode();

  //SerialWriter writer("/dev/ttyACM0"); 
  //Heda heda(writer);

  //calibrateShelvesHeight(workingShelfHeight);
  //calibrateCamWithJarRef(heda);

  Mat frame;
  heda.captureFrame(frame);

  // TODO: Latter take multiple images have a reference. Go up and down to take many images.
  cerr << "WARNING: Assuming camera to jar label distance to be 202 mm.\n";
  double cameraDistance = 202;

  double pixelsPerMm = code.code.scale;
  cerr << "Pixels per mm: " << code.code.scale << endl;

  double codePixelsWidth = pixelsPerMm * HR_CODE_WIDTH;
  double focalPoint = computeFocalPoint(codePixelsWidth, cameraDistance, HR_CODE_WIDTH);

  cout << "#endif\n";

  return 0;
}
