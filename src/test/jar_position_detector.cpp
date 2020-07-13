#include "test.h"

#include "../core/jar_position_detector.h"
#include "../utils/io_common.h"
#include "../core/sweep.h"

void testImageDelta() {
}

int main (int argc, char *argv[]) {

  signal(SIGINT, signalHandler);

  DetectedHRCode code();

  //FOUND (127.335, 142.398)[640, 480]{1.16312} at (320, 0, 85)
//FOUND (363.636, 238.243)[640, 480]{1.19391} at (320, 0, 255)
//FOUND (366.351, 238.047)[640, 480]{1.18762} at (160, 0, 425)
//FOUND (365.808, 238.269)[640, 480]{1.19326} at (160, 0, 255)
//FOUND (366.035, 237.621)[640, 480]{1.19183} at (160, 0, 170)


  testImageDelta();

}
