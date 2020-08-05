#ifndef _SERIAL_READER_H
#define _SERIAL_READER_H

#include "reader.h"

class GetByteOnEmptyStreamException : public exception {};
class EmptyInputInvalidException : public exception {};

class SerialReader : public Reader {
  public:
    
    SerialReader(SerialPort& p) : m_port(p) {
    }

    bool inputAvailable() {

      if (m_separator_found) {
        return true;
      } else {
        char msg[256]; 
        memset(&msg, '\0', sizeof(msg));
        int num_bytes = m_port.readBytes(msg, sizeof(msg));
        if (num_bytes > 0) {
          m_input += msg;
          searchForSeparator();
        }
      }
      return m_separator_found;
    }

    int getByte() {
        
      if (!inputAvailable()) { // This logic should be handled by Reader
        throw GetByteOnEmptyStreamException();
      }

      if (m_input.empty()) {throw EmptyInputInvalidException();} // Should never happen, but to be safe. It's clearer than out_or_range.

      char c = m_input.at(0);
      m_input.erase(m_input.begin());

      if (c == m_separator) {
        searchForSeparator();
      }

      return c;
    }

  protected:

    void searchForSeparator() {
      m_separator_found = m_input.find(m_separator) != string::npos;
    }

    bool m_separator_found = false;
    char m_separator = '\n';
    SerialPort& m_port;
    string m_input;
};

#endif
