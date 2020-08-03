#ifndef STRING_WRITER
#define STRING_WRITER

#include <iostream>
#include "writer.h"
#include "../lib/serial.h"

using namespace std;

class StringWriter : public Writer {
  public:

    SerialWriter(const char* port) {

      cerr << "Opening port " << port << "..\n";
      SerialPort m_port;
      if (m_port.openPort(port) < 0) {
        throw InitSerialPortException();
      }
    }

    void doPinMode(int pin, bool type) {
    }
    void doDigitalWrite(int pin, bool value) {
    }
    double doDigitalRead(int pin) {
      return 0.0;
    }
    void doPrint(const char* val) {
      //if (!m_port) {throw NullPortException();}
      m_port.writePort(val);
    }
    void doPrint(char val) {
      m_port.writePort(to_string(val));
    }
    void doPrint(double val) {
      m_port.writePort(to_string(val));
    }
    void doPrint(long val) {
      m_port.writePort(to_string(val));
    }
    void doPrint(unsigned long val) {
      m_port.writePort(to_string(val));
    }
    void doPrint(bool val) {
      m_port.writePort(to_string(val));
    }
    void doPrint(int val) {
      m_port.writePort(to_string(val));
    }

    SerialPort& m_port;
};

#endif
