#ifndef _ECHO_WRITER_H
#define _ECHO_WRITER_H

#include "std_writer.h"

using namespace std;

class EchoWriter : public StdWriter {
  public:
    EchoWriter(string name, Writer& writer) : name(name), writer(writer) {}
    
    void doPrint(string str) {
      cout << name << ": " << str << endl;
      writer << str.c_str();
    }

    string name;
    Writer& writer;
};

#endif
