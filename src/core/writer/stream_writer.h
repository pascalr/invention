#ifndef STREAM_WRITER
#define STREAM_WRITER

#include <sstream>
#include "std_writer.h"

class StreamWriter : public StdWriter {
  public:

    std::string str() {
      std::string str = ss.str();
      ss.str("");
      ss.clear();
      return str;
    }
    
    void doPrint(std::string str) {
      ss << str;
    }

    std::stringstream ss;
};

#endif
