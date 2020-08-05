#ifndef _SERIAL_READER_H
#define _SERIAL_READER_H

#include "reader.h"

class EmptyInputInvalidException : public exception {};

class SerialReader : public Reader {
  public:
    
    SerialReader(SerialPort& p) : m_port(p) {
    }

    bool inputAvailable() {
      return !m_input.empty() || m_port.inputAvailable();
    }

    int getByte() {

      if (m_input.empty()) {
        if (!m_port.inputAvailable()) {
          throw GetByteOnEmptyStreamException();
        }
        m_port.getInput(m_input);
      }

      if (m_input.empty()) {throw EmptyInputInvalidException();} // Should never happen but to be safe..

      char c = m_input[0];
      m_input.erase(m_input.begin());
      return c;
    }

  protected:

    SerialPort& m_port;
    string m_input;
};

#endif
