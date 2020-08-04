#ifndef CONSOLE_WRITER
#define CONSOLE_WRITER

#include <iostream>
#include "std_writer.h"

class ConsoleWriter : public StdWriter {
  public:
    void doPrint(string val) {
      std::cout << val;
    }
};

#endif
