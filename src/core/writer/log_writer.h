#ifndef _LOG_WRITER_H
#define _LOG_WRITER_H

#include "std_writer.h"

#include <iostream>

class LogWriter : public StdWriter {
  public:
    LogWriter(const std::string& name) : name(name), writer(0) {}
    LogWriter(const std::string& name, Writer& writer) : name(name), writer(&writer) {}
    
    void doPrint(std::string str) {
      std::cout << name << ": " << str << std::endl;
      if (writer != 0) {
        *writer << str.c_str();
      }
    }

    std::string name;
    Writer* writer;
};

#endif
