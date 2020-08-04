#ifndef SERIAL_WRITER
#define SERIAL_WRITER

#include <iostream>
#include "writer.h"
#include "../lib/serial.h"

using namespace std;

class SerialWriter : public Writer {
  public:

    SerialWriter(SerialPort& p) : m_port(p) {
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
