#include "reader.h"

#include <string>
#include <thread>
#include <chrono>

#include "../../lib/lib.h"

std::string getInputLine(Reader& r) {
  std::string str;

  //while (r.inputAvailable()) {
  //  char c = r.getByte();
  //  str += c;
  //  if (c == '\n') {break;}
  //}
  while (true) {
    if (r.inputAvailable()) {
      char c = r.getByte();
      str += c;
      if (c == '\n') {break;}
    } else {
      std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
  }
  trim(str);
  return str;
}

std::string getAllAvailable(Reader& r) {

  std::string str;
  while (r.inputAvailable()) {
    char c = r.getByte();
    str += c;
  }
  return str;
}
