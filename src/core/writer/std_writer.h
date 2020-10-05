#ifndef _STD_WRITER_H
#define _STD_WRITER_H

#include "writer.h"
#include <string>

class StdWriter : public Writer {
  public:
    virtual void doPrint(std::string str) = 0;

    void doPrint(const char* val) {
      doPrint((std::string)val);
    }

    void doPrint(char val) {
      std::string s = ""; s += val;
      doPrint(s);
    }

    void doPrint(double val) {
      doPrint(std::to_string(val));
    }
    void doPrint(long val) {
      doPrint(std::to_string(val));
    }
    void doPrint(bool val) {
      doPrint(std::to_string(val));
    }
    void doPrint(unsigned long val) {
      doPrint(std::to_string(val));
    }
};

#endif
