#include "core/reader/serial_reader.h"
#include "core/writer/serial_writer.h"

#include <thread>
#include <chrono>

#include <iostream>

int main() {
  
  SerialPort serialPort;
  if (serialPort.openPort("/dev/tty/ACM0") < 0) {
    std::cerr << "Error cannot open /dev/tty/ACM0. Is arduino plugged in?\n";
    return -1;
  }

  SerialReader serialReader(serialPort);
  SerialWriter serialWriter(serialPort);

  std::this_thread::sleep_for(std::chrono::seconds(8));

  serialWriter << "#";

  for (int i = 0; i < 50; i++) {
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    if (serialReader.inputAvailable()) {
      std::string str = getAllAvailable(serialReader);
      std::cout << str;
      return 0;
    }
  }

  return -1;
}

