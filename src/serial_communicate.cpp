#include "lib/serial.h"
#include <iostream>
#include <string>

#include "utils/io_common.h"

#include <chrono>
#include <thread>

using namespace std;

int main(int argc, char** argv) {

  signal(SIGINT, signalHandler);

  SerialPort p;
  if (p.openPort("/dev/ACM0") < 0) {
    cout << "Error opening arduino port. Aborting." << endl;
    return -1;
  }

  while (true) {
    if (linuxInputAvailable()) {
      string s;
      cin >> s;
      p.writePort(s.c_str());
    } else if (p.inputAvailable()) {
      string s;
      p.getInput(s);
      cout << s;
    }
    this_thread::sleep_for(chrono::milliseconds(10));
  }

  return 0;
}
