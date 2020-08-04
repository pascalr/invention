#ifndef STREAM_WRITER
#define STREAM_WRITER

#include "std_writer.h"

using namespace std;

class StreamWriter : public StdWriter {
  public:

    string str() {
      string str = ss.str();
      ss.str("");
      ss.clear();
      return str;
    }
    
    void doPrint(string str) {
      ss << str;
    }

    stringstream ss;
};

#endif
