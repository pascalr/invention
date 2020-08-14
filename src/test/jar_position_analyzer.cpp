#include "test.h"

#include "../core/jar_position_analyzer.h"
#include "../utils/io_common.h"
#include "../core/sweep.h"
#include "../config/constants.h"

#include "../lib/hr_code.h"
#include "../lib/opencv.h"

using namespace std;

/*void testCameraPosition() {
  title("Testing camera position");

  Vector2d toolPosition;
  toolPosition << BASE_TOOL_DISTANCE, 0;
  Vector2d cameraPos = cameraPosition(toolPosition, 0);

  assertNearby("0 degree, x", BASE_CAMERA_DISTANCE, cameraPos(0));
  assertNearby("0 degree, z", 0.0, cameraPos(1));
  
  Vector2d toolPosition90;
  toolPosition90 << 0, BASE_TOOL_DISTANCE;
  Vector2d cameraPos90 = cameraPosition(toolPosition90, 90);

  assertNearby("90 degree, x", 0.0, cameraPos90(0));
  assertNearby("90 degree, z", BASE_CAMERA_DISTANCE, cameraPos90(1));
}

void testImageDelta() {
}

void testTransform() {
  title("Testing transform");

  Transform<double, 2, TransformTraits::Affine> t(Rotation2D<double>(90 / 180 * PI));

  Vector2d val;
  val << 100.0, 0.0;

  //val*=jarCenter = t * jarCenter;
}

void testJarOffset() {
  title("Testing jar offset");
  
  Vector2d result;

  Vector2d imgCenterLeftOfImage;
  imgCenterLeftOfImage << CAMERA_WIDTH/4, CAMERA_HEIGHT/2; 

  double pixelsPerMm = 10;
  double pixelsOffset = CAMERA_WIDTH/4;
  double offsetMm = pixelsOffset / pixelsPerMm;

  // At 0 degree, left of image is z positive
  result = jarOffset(imgCenterLeftOfImage, 0.0, pixelsPerMm);
  assertNearby("0 degree, x", 0.0, result(0));
  assertNearby("0 degree, z", offsetMm, result(1));

  // At 90 degree, left of image is x negative
  result = jarOffset(imgCenterLeftOfImage, 90.0, pixelsPerMm);
  assertNearby("0 degree, x", -offsetMm, result(0));
  assertNearby("0 degree, z", 0.0, result(1));

}*/

void testConvertToAbsolutePosition() {
  title("Testing convert to absolute position");


  /*Vector2d toolPosition;
  toolPosition << BASE_TOOL_DISTANCE, 0;
  Vector2d cameraPos = cameraPosition(toolPosition, 0);

  double angle = 0;

  Vector2d imgCenter;
  double scale;*/
}

int main (int argc, char *argv[]) {

  signal(SIGINT, signalHandler);

  /*Mat mat = imread("data/test/pot_01.png", IMREAD_GRAYSCALE);

  HRCode code1(mat, 285.669, 198.225, 2.78717);
  HRCode code2(mat, 361.880, 233.758, 2.7581);
  HRCode code3(mat, 361.822, 234.178, 2.76698);
  HRCode code4(mat, 360.562, 233.585, 2.76782);
  HRCode code5(mat, 361.265, 233.735, 2.76808);
  HRCode code6(mat, 361.000, 234.000, 2.77495);
    
  DetectedHRCode detected1(code1, 320, 0, 170, 30.0);
  DetectedHRCode detected2(code2, 320, 0, 255, 48.6);
  DetectedHRCode detected3(code3, 320, 0, 340, 90.0);
  DetectedHRCode detected4(code4, 160, 0, 340, 90.0);
  DetectedHRCode detected5(code5, 160, 0, 255, 131.4);
  DetectedHRCode detected6(code5, 160, 0, 170, 150.0);

  vector<DetectedHRCode> detected{detected1, detected2, detected3, detected4, detected5, detected6};
  vector<PositionedHRCode> result;

  JarPositionAnalyzer analyzer;
  analyzer.run(detected, result);

  // The expected result is about 410, -30

  cout << "Results:" << endl;
  for (auto it = result.begin(); it != result.end(); it++) {
    cout << "ANALYZED " << *it << std::endl;
  }

  //FOUND (127.335, 142.398)[640, 480]{1.16312} at (320, 0, 85)
  //FOUND (363.636, 238.243)[640, 480]{1.19391} at (320, 0, 255)
  //FOUND (366.351, 238.047)[640, 480]{1.18762} at (160, 0, 425)
  //FOUND (365.808, 238.269)[640, 480]{1.19326} at (160, 0, 255)
  //FOUND (366.035, 237.621)[640, 480]{1.19183} at (160, 0, 170)

  testCameraPosition();
  testConvertToAbsolutePosition();
  testJarOffset();
  testImageDelta();*/

}
