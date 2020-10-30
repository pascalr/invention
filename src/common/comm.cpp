#include "comm.h"

#include <thread>
#include <chrono>
#include <iostream>

bool isArduinoReady(Reader &reader) {

  double timeoutS = 20.0;
  double sleepTimeMs = 50.0;
  int nbAttempts = timeoutS * 1000.0 / sleepTimeMs;

  for (int i = 0; i < nbAttempts; i++) {
    if (reader.inputAvailable()) {
      std::string cmd = getInputLine(reader);
      if (cmd == "ready") return true;
    }
    std::this_thread::sleep_for(std::chrono::milliseconds((int)sleepTimeMs));
  }
  return false;
}

void waitUntilDone(Reader& reader) {
  // Loops until the message "done" has been received.
  std::cout << "Waiting until receiving message done.\n";
  while (true) {
    if (reader.inputAvailable()) {
      std::string cmd = getInputLine(reader);
      if (cmd == "done") {
        std::cout << "OK. done.\n";
        return;
      }
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(20));
  }
}

std::string readAllUntilDone(Reader& reader) {
  std::string str;
  std::cout << "Waiting until receiving message done.\n";
  while (true) {
    if (reader.inputAvailable()) {
      std::string cmd = getInputLine(reader);
      str += cmd;
      if (cmd == "done") {
        std::cout << "OK. done.\n";
        return str;
      }
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(20));
  }
}
