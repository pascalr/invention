#ifndef STRING_WRITER 
#define STRING_WRITER

#include <iostream>
#include <string>
#include "../core/writer.h"

using namespace std;

class StringWriter : public Writer {
  public:
    void getString(string& str) {
      str.assign(m_ss.str());
      m_ss.clear();
    }
    void doPinMode(int pin, bool type) {
    }
    void doDigitalWrite(int pin, bool value) {
    }
    double doDigitalRead(int pin) {
    }
    void doPrint(const char* val) {
      m_ss << val;
    }
    void doPrint(char val) {
      m_ss << val;
    }
    void doPrint(double val) {
      m_ss << val;
    }
    void doPrint(long val) {
      m_ss << val;
    }
    void doPrint(unsigned long val) {
      m_ss << val;
    }
    void doPrint(bool val) {
      m_ss << val;
    }
    void doPrint(int val) {
      m_ss << val;
    }
  private:
    stringstream m_ss;
};

#endif
