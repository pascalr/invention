#ifndef CONSOLE_WRITER
#define CONSOLE_WRITER

#include <iostream>

using namespace std;

class ConsoleWriter : public Writer {
  public:
    void doPinMode(int pin, bool type) {
      //cerr << "pinMode(" << pin << ", " << type << ")" << endl;
    }
    void doDigitalWrite(int pin, bool value) {
      //cerr << "digitalWrite(" << pin << ", " << value << ")" << endl;
    }
    double doDigitalRead(int pin) {
      //cerr << "digitalRead(" << pin << ")" << endl;
    }
    void doPrint(const char* val) {
      cerr << val;
      cout << val;
    }
    void doPrint(char val) {
      cerr << val;
      cout << val;
    }
    void doPrint(double val) {
      cerr << val;
      cout << val;
    }
    void doPrint(long val) {
      cerr << val;
      cout << val;
    }
    void doPrint(unsigned long val) {
      cerr << val;
      cout << val;
    }
    void doPrint(bool val) {
      cerr << val;
      cout << val;
    }
    void doPrint(int val) {
      cerr << val;
      cout << val;
    }
};

#endif
