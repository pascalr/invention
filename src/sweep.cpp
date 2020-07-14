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
  // setupLogging();

  cerr << "Opening arduino port...";
  SerialPort p;
  if (p.openPort("/dev/ttyACM0") < 0) {
    cerr << "Error opening arduino port.";
    return -1;
  }

  cerr << "Doing reference...";
  p.writePort("H");
  cerr << "Waiting until receives message done...";
  p.waitUntilMessageReceived(MESSAGE_DONE);

  cerr << "Reference done. Sweeping...";
  Sweep sweep(p);
  if(!sweep.init()) {
    cerr << "Error initializing sweep.\n";
    return -1;
  }

  vector<DetectedHRCode> candidates;
  sweep.run(candidates);

  for (int i = 0; i < candidates.size(); i++) {
    HRCode& code = candidates[i].code;
    cout << "HRCode code" << i << "(mat, " << code.x << ", " << code.y << ", " << code.scale << ");\n";
    cout << "DetectedHRCode detected" << i << "(code" << i << ", ";
    cout << candidates[i].x << ", " << candidates[i].y << ", " << candidates[i].z << ", " << candidates[i].angle << ");\n";
  }

  return 0;
}


