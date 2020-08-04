#include <iostream>

#include "core/Heda.h"
#include "core/writer/serial_writer.h"

using namespace std;

//FOUND (127.335, 142.398)[640, 480]{1.16312} at (320, 0, 85)
//FOUND (363.636, 238.243)[640, 480]{1.19391} at (320, 0, 255)
//FOUND (366.351, 238.047)[640, 480]{1.18762} at (160, 0, 425)
//FOUND (365.808, 238.269)[640, 480]{1.19326} at (160, 0, 255)
//FOUND (366.035, 237.621)[640, 480]{1.19183} at (160, 0, 170)

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

void writeConfig(string name, string value) {
  cout << "#define " << name << " " << value << "\n";
}

/*void calibrateShelvesHeight(double &workingShelfHeight) {
  cerr << "TODO: calibrate shelves heights: The tooling is really simply. Grab a stick that sticks out. Force the stick down"
          " until a resistance is felt.\n";

  workingShelfHeight = 350.0;
  writeConfig("WORKING_SHELF_HEIGHT", to_string(workingShelfHeight));
  writeConfig("SHELVES_HEIGHT", "[150.0, 350.0]");
  writeConfig("SHELVES_TOTAL", to_string(2));
}*/

void calibrateCamWithJarRef(Heda& heda) {
  
  char key;

  cerr << "Please clear the working shelf. Press any key when done..";
  cin >> key;

  cerr << "The shelves height must already be valid. Press any key to confirm..";
  cin >> key;

  cerr << "The tool in it's hand must a grabber. Press any key to confirm..";
  cin >> key;

  cerr << "To configure the camera, a reference jar with a sticker must be used. Please enter the reference jar height in mm.\n";
  double height;
  cin >> height;

  cerr << "Moving to center of working shelf, at height=(" << height << "+4) mm.\n";

  //Vector3d destination;
  //destination << TOOL_X_MIDDLE, workingShelfHeight + height + 4, TOOL_Z_MIDDLE;

  heda.reference(); // Make sure it is referenced.
  heda.move(Movement('T', CHANGE_LEVEL_ANGLE_LOW));
  heda.move(Movement('Y', WORKING_SHELF_HEIGHT+height+4));
  heda.move(Movement('T', 30.0));

  // Find the position


  // FIXME: Move directly, so the angle is known. Move to requires CLAW_RADIUS which must be calculated.

  // TODO: Calculate claw radius.
  writeConfig("CLAW_RADIUS", to_string(340.0));
  
 
  /*heda.release();

  cerr << "Please add the reference jar inside the jaws. Press any key when ready..";
  cin >> key;

  destination.y = MAX_WORKING_HEIGHT;
  heda.moveTo(destination);

  DetectedHRCode code = heda.detectOneCode();*/
}

int main(int argc, char** argv)
{ // 21 mm and 31 mm

  //SerialWriter writer("/dev/ttyACM0"); 
  //Heda heda(writer);

  //calibrateShelvesHeight(workingShelfHeight);
  //calibrateCamWithJarRef(heda);

  /*Mat frame;
  heda.captureFrame(frame);

  // TODO: Latter take multiple images have a reference. Go up and down to take many images.
  cerr << "WARNING: Assuming camera to jar label distance to be 202 mm.\n";
  double cameraDistance = 202;

  DetectedHRCode code = heda.detectOneCode();
  double pixelsPerMm = code.code.scale;
  cerr << "Pixels per mm: " << code.code.scale << endl;

  double codePixelsWidth = pixelsPerMm * HR_CODE_WIDTH;
  double focalPoint = computeFocalPoint(codePixelsWidth, cameraDistance, HR_CODE_WIDTH);

  //ofstream cout;
  //cout.open ("src/config/camera_constants.h");
  cout << "// Autogenerated by camera_calibration.\n";
  cout << "\n";
  cout << "#ifndef CAMERA_CONSTANTS\n";
  cout << "#define CAMERA_CONSTANTS\n";
  cout << "\n";
  cout << "#define CAMERA_WIDTH " << frame.cols << "\n";
  cout << "#define CAMERA_HEIGHT " << frame.rows << "\n";

  //cout << "#define CAMERA_FIELD_OF_VIEW 75.0 // degrees\n";
  // TODO: Camera focal point
  cout << "#define CAMERA_FOCAL_POINT " << focalPoint << "\n";
  cout << "\n";
  cout << "#endif\n";*/

  return 0;
}
