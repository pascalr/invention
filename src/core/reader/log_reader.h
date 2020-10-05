#ifndef _LOG_READER_H
#define _LOG_READER_H

#include "reader.h"

class LogReader : public Reader {
  public:
    LogReader(const std::string& name, Reader& reader) : m_name(name), m_reader(reader) {}

    int getByte() {
      if (m_was_newline) {
        std::cout << m_name << ": ";
        m_was_newline = false;
      }
      int byte = m_reader.getByte();
      std::cout << (char)byte;
      if (byte == '\n') {
        m_was_newline = true;
      }
      return byte;
    }
    
    bool inputAvailable() {
      return m_reader.inputAvailable();
    }

    bool m_was_newline = true;
    std::string m_name;
    Reader& m_reader;

};

#endif
