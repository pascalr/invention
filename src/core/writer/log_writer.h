#ifndef _LOG_WRITER_H
#define _LOG_WRITER_H

#include "std_writer.h"

using namespace std;

class LogWriter : public StdWriter {
  public:
    LogWriter(const string& name, Writer& writer) : name(name), writer(writer) {}
    
    void doPrint(string str) {
      cout << name << ": " << str << endl;
      writer << str.c_str();
    }

    string name;
    Writer& writer;
};

#endif
