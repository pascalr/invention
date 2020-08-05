#ifndef CONSOLE_WRITER
#define CONSOLE_WRITER

#include <iostream>
#include "std_writer.h"
#include <string>

class ConsoleWriter : public StdWriter {
  public:
    void doPrint(std::string val) {
      std::cout << val;
    }
};

#endif
