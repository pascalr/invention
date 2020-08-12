#ifndef _TWO_WAY_STREAM_H
#define _TWO_WAY_STREAM_H

#include "writer/std_writer.h"
#include "reader/reader.h"
#include <sstream>

using namespace std;

class TwoWayStream : public StdWriter, public Reader {
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
    
    bool inputAvailable() {
      return ss.rdbuf()->in_avail();
    }

    int getByte() {
      return ss.get();
    }

    stringstream ss;
};

#endif
