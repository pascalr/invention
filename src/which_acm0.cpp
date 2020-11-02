#include "core/reader/serial_reader.h"
#include "core/writer/serial_writer.h"
#include "lib/lib.h"
#include "core/comm.h"

#include <thread>
#include <chrono>

#include <iostream>

void clearSerial(SerialReader &reader) {
  while (reader.inputAvailable()) reader.getByte();
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
  std::string v = getArduinoVersion(serialWriter, serialReader);

  std::cout << "ACM0: " << v << "\n";
  return -1;
}

