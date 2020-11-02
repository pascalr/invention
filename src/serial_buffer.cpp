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
  if (serialPort.openPort("/tmp/heda0") < 0) {
    std::cerr << "Error cannot open /dev/tty/ACM0. Is arduino plugged in?\n";
    return -1;
  }

  SerialReader serialReader(serialPort);

  while (true) {
  }

  return -1;
}

