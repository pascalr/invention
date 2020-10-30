#include "core/reader/serial_reader.h"
#include "core/writer/serial_writer.h"
#include "lib/lib.h"

#include <thread>
#include <chrono>

#include <iostream>

void clearSerial(SerialReader &reader) {
  while (reader.inputAvailable()) reader.getByte();
}

bool isArduinoReady(SerialReader &reader) {

  double timeoutS = 20.0;
  double sleepTimeMs = 50.0;
  int nbAttempts = timeoutS * 1000.0 / sleepTimeMs;

  for (int i = 0; i < nbAttempts; i++) {
    if (reader.inputAvailable()) {
      //std::string str = getAllAvailable(reader);
      std::string cmd = getInputLine(reader);
      //std::cerr << cmd << "\n";
      if (cmd == "ready") return true;
    }
    std::this_thread::sleep_for(std::chrono::milliseconds((int)sleepTimeMs));
  }
  return false;
}

int main() {
  
  SerialPort serialPort;
  if (serialPort.openPort("/dev/ttyACM0") < 0) {
    std::cerr << "Error cannot open /dev/tty/ACM0. Is arduino plugged in?\n";
    return -1;
  }

  SerialReader serialReader(serialPort);
  SerialWriter serialWriter(serialPort);

  clearSerial(serialReader);

  if (!isArduinoReady(serialReader)) {
    std::cerr << "Error arduino response timeout. Was not ready.\n";
    return -1;
  }

  serialWriter << "#";
  
  double timeoutS = 20.0;
  double sleepTimeMs = 50.0;
  int nbAttempts = timeoutS * 1000.0 / sleepTimeMs;

  for (int i = 0; i < nbAttempts; i++) {
    if (serialReader.inputAvailable()) {
      //std::string str = getAllAvailable(serialReader);
      std::string cmd = getInputLine(serialReader);
      trim(cmd);
      if (cmd == "fixed" || cmd == "mobile") {
        std::cout << cmd;
        return 0;
      }
    }
    std::this_thread::sleep_for(std::chrono::milliseconds((int)sleepTimeMs));
  }

  std::cerr << "Timeout reached. Arduino did not respond to the version (#) request.\n";
  return -1;
}

