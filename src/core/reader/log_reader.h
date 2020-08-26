#ifndef _LOG_READER_H
#define _LOG_READER_H

#include "reader.h"

using namespace std;

class LogReader : public Reader {
  public:
    LogReader(const string& name, Reader& reader) : m_name(name), m_reader(reader) {}

    int getByte() {
      if (m_first_get_byte) {
        cout << m_name << ": ";
        m_first_get_byte = false;
      }
      int byte = m_reader.getByte();
      cout << (char)byte;
      if (byte == '\n') {
        cout << m_name << ": ";
      }
      return byte;
    }
    
    bool inputAvailable() {
      return m_reader.inputAvailable();
    }

    bool m_first_get_byte = true;
    std::string m_name;
    Reader& m_reader;

};

#endif
