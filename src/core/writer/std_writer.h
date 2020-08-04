#ifndef _STD_WRITER_H
#define _STD_WRITER_H

class StdWriter : public Writer {
  public:
    virtual void doPrint(string str) = 0;

    void doPrint(const char* val) {
      doPrint((string)val);
    }

    void doPrint(char val) {
      doPrint(to_string(val));
    }

    void doPrint(double val) {
      doPrint(to_string(val));
    }
    void doPrint(long val) {
      doPrint(to_string(val));
    }
    void doPrint(bool val) {
      doPrint(to_string(val));
    }
    void doPrint(unsigned long val) {
      doPrint(to_string(val));
    }
};

#endif
