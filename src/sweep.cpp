#include "utils/io_common.h"
#include "lib/lib.h"
#include "lib/hr_code.hpp"
#include "lib/serial.h"
#include "config/setup.h"
#include "helper/helper.h"

#include "core/sweep.h"

// x and z position is the position of the camera.
int main(int argc, char *argv[])
{
  setupInterrupt();
  setupLogging();

  /*Sweep sweep;
  if(!sweep.init()) {
    cerr << "Error initializing sweep.\n";
    return -1;
  }
  sweep.run();*/

  return 0;
}


