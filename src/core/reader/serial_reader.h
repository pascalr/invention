#ifndef _SERIAL_READER_H
#define _SERIAL_READER_H

#include "reader.h"

class SerialReader : public Reader {
  public:
    
    SerialReader(SerialPort& p) : m_port(p) {
    }

    bool inputAvailable() {
      return m_port.inputAvailable();
    }

    int getByte() {

      if (m_input.empty()) {
        m_port.lock(SERIAL_KEY_READER);
        m_port.getInput(m_input);
        m_port.unlock();
      }

      if (m_input.empty()) {return -1;}

      char c = m_input[0];
      m_input.erase(m_input.begin());
      return c;
    }

  protected:

    SerialPort& m_port;
    string m_input;
};

#endif
