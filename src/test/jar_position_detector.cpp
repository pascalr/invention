#include "test.h"

#include "../core/jar_position_detector.h"
#include "../utils/io_common.h"
#include "../core/sweep.h"

#include "../lib/hr_code.hpp"
#include "../lib/opencv.h"

void testImageDelta() {
}

int main (int argc, char *argv[]) {

  signal(SIGINT, signalHandler);

  Mat mat = imread("data/test/pot_01.png", IMREAD_GRAYSCALE);

  HRCode code1(mat, 127.335, 142.398, 1.16312);
  HRCode code2(mat, 363.636, 238.243, 1.19361);
  HRCode code3(mat, 366.351, 238.047, 1.18762);
  HRCode code4(mat, 365.808, 238.269, 1.19326);
  HRCode code5(mat, 366.035, 237.621, 1.19183);
    
  DetectedHRCode detected1(code1, 320, 0, 85, 0.0);
  DetectedHRCode detected2(code2, 320, 0, 255, 0.0);
  DetectedHRCode detected3(code3, 160, 0, 425, 0.0);
  DetectedHRCode detected4(code4, 160, 0, 255, 0.0);
  DetectedHRCode detected5(code5, 160, 0, 170, 0.0);

  DetectedHRCode code();

  //FOUND (127.335, 142.398)[640, 480]{1.16312} at (320, 0, 85)
  //FOUND (363.636, 238.243)[640, 480]{1.19391} at (320, 0, 255)
  //FOUND (366.351, 238.047)[640, 480]{1.18762} at (160, 0, 425)
  //FOUND (365.808, 238.269)[640, 480]{1.19326} at (160, 0, 255)
  //FOUND (366.035, 237.621)[640, 480]{1.19183} at (160, 0, 170)

  testImageDelta();

}
