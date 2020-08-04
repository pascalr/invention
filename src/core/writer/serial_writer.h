#ifndef SERIAL_WRITER
#define SERIAL_WRITER

#include <iostream>
#include "std_writer.h"
#include "../../lib/serial.h"

using namespace std;

class SerialWriter : public StdWriter {
  public:

    SerialWriter(SerialPort& p) : m_port(p) {
    }

    void doPrint(string val) {
      m_port.writePort(val);
    }

    SerialPort& m_port;
};

#endif
