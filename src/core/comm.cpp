#include "comm.h"

#include <thread>
#include <chrono>
#include <iostream>
#include <exception>

#include "../lib/lib.h"

//bool isArduinoReady(Reader &reader) {
//
//  double timeoutS = 20.0;
//  double sleepTimeMs = 50.0;
//  int nbAttempts = timeoutS * 1000.0 / sleepTimeMs;
//
//  for (int i = 0; i < nbAttempts; i++) {
//    if (reader.inputAvailable()) {
//      std::string cmd = getInputLine(reader);
//      if (cmd == "ready") return true;
//    }
//    std::this_thread::sleep_for(std::chrono::milliseconds((int)sleepTimeMs));
//  }
//  return false;
//}

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

std::string getArduinoVersion(Writer& writer, Reader &reader) {
  for (int i = 0; i < 20; i++) {
    writer << "#";
    std::string r;
    if (readProperty("#", r, reader, 50, 20)) return r;
  }
  throw std::runtime_error("GetArduinoVersion timeout reached.");
}

bool readProperty(const std::string &name, std::string &result, Reader &reader, int nbAttempts, long sleepMs) {

  for (int i = 0; i < nbAttempts; i++) {
    while (reader.inputAvailable()) {
      std::string cmd = getInputLine(reader);
      if (startsWith(cmd, name+":")) {
        result = cmd.substr(name.size()+1);
        trim(result);
        return true;
      }
    }
    std::this_thread::sleep_for(std::chrono::milliseconds((int)sleepMs));
  }
  return false;
}

