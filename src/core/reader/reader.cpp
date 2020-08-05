#include "reader.h"

#include <string>

#include "../../lib/lib.h"

std::string getInputLine(Reader& r) {
  std::string str;
  while (r.inputAvailable()) {
    char c = r.getByte();
    str += c;
    if (c == '\n') {break;}
  }
  trim(str);
  return str;
}
