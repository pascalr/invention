#ifndef CONSOLE_WRITER
#define CONSOLE_WRITER

#include <iostream>

using namespace std;

class ConsoleWriter : public Writer {
  public:
    void doPinMode(int pin, bool type) {
      cerr << "pinMode(" << pin << ", " << type << ")" << endl;
    }
    void doDigitalWrite(int pin, bool value) {
      cerr << "digitalWrite(" << pin << ", " << value << ")" << endl;
    }
    double doDigitalRead(int pin) {
      cerr << "digitalRead(" << pin << ")" << endl;
    }
    void doPrint(const char* theString) {
      cerr << theString;
    }
    void doPrint(char val) {
      cerr << val;
    }
    void doPrint(double val) {
      cerr << val;
    }
    void doPrint(long val) {
      cerr << val;
    }
    void doPrint(unsigned long val) {
      cerr << val;
    }
    void doPrint(bool val) {
      cerr << val;
    }
    void doPrint(int val) {
      cerr << val;
    }
};

#endif
