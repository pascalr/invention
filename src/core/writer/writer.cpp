#include "writer.h"

Writer& operator<<(Writer& writer, const char* theString) {
  writer.doPrint(theString);
  return writer;
}

Writer& operator<<(Writer& writer, char val) {
  writer.doPrint(val);
  return writer;
}

Writer& operator<<(Writer& writer, double val) {
  writer.doPrint(val);
  return writer;
}

Writer& operator<<(Writer& writer, long val) {
  writer.doPrint(val);
  return writer;
}

Writer& operator<<(Writer& writer, unsigned long val) {
  writer.doPrint(val);
  return writer;
}

Writer& operator<<(Writer& writer, bool val) {
  writer.doPrint(val);
  return writer;
}

Writer& operator<<(Writer& writer, int val) {
  writer.doPrint((long)val);
  return writer;
}

#ifndef ARDUINO
Writer& operator<<(Writer& writer, std::string val) {
  return writer << val.c_str();
}
#endif
