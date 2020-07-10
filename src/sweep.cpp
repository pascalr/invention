#include "utils/io_common.h"
#include "lib/lib.h"
#include "lib/hr_code.hpp"
#include "lib/serial.h"
#include "config/setup.h"
#include "helper/logging.h"

#include "core/sweep.h"

// x and z position is the position of the camera.
int main(int argc, char *argv[])
{
  setupInterrupt();
  setupLogging();

  SerialPort p;
  if (p.openPort("/dev/ttyACM0") < 0) {
    cerr << "Error opening arduino port.";
    return -1;
  }
  cout << "Doing reference...";
  p.writePort("H");
  p.waitUntilMessageReceived(MESSAGE_DONE);

  cout << "Reference done. Sweeping...";
  Sweep sweep(p);
  if(!sweep.init()) {
    cerr << "Error initializing sweep.\n";
    return -1;
  }

  vector<DetectedHRCodePosition> candidates;
  sweep.run(candidates);

  for (auto it = candidates.begin(); it != candidates.end(); it++) {
    cout << "FOUND " << *it << std::endl;
  }

  return 0;
}


