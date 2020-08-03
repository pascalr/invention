#ifndef STREAM_WRITER
#define STREAM_WRITER

#include "writer.h"

using namespace std;

class StreamWriter : public Writer {
  public:

    string str() {
      string str = ss.str();
      ss.str("");
      ss.clear();
      return str;
    }

    void doPinMode(int pin, bool type) {
    }
    void doDigitalWrite(int pin, bool value) {
    }
    double doDigitalRead(int pin) {
      return 0.0;
    }
    void doPrint(const char* val) {
      ss << val;
    }
    void doPrint(char val) {
      ss << val;
    }
    void doPrint(double val) {
      ss << val;
    }
    void doPrint(long val) {
      ss << val;
    }
    void doPrint(unsigned long val) {
      ss << val;
    }
    void doPrint(bool val) {
      ss << val;
    }
    void doPrint(int val) {
      ss << val;
    }

    stringstream ss;
};

#endif
