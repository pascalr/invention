#ifndef _TWO_WAY_STREAM_H
#define _TWO_WAY_STREAM_H

#include "writer/std_writer.h"
#include "reader/reader.h"
#include <sstream>

#include <mutex>

class TwoWayStream : public StdWriter, public Reader {
  public:

    std::string str() {
      std::lock_guard<std::mutex> guard(streamMutex);
      std::string str = ss.str();
      ss.str("");
      ss.clear();
      return str;
    }
    
    void doPrint(std::string str) {
      std::lock_guard<std::mutex> guard(streamMutex);
      ss << str << "\n";
    }
    
    bool inputAvailable() {
      std::lock_guard<std::mutex> guard(streamMutex);
      return ss.rdbuf()->in_avail();
    }

    int getByte() {
      std::lock_guard<std::mutex> guard(streamMutex);
      return ss.get();
    }

    std::stringstream ss;
   
    std::mutex streamMutex;
};

#endif
