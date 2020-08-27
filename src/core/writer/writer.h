#ifndef WRITER_H
#define WRITER_H

class Writer {
  public:
    virtual void doPrint(const char* val) = 0;
    virtual void doPrint(char val) = 0;
    virtual void doPrint(double val) = 0;
    virtual void doPrint(long val) = 0;
    virtual void doPrint(bool val) = 0;
    virtual void doPrint(unsigned long val) = 0;
};

Writer& operator<<(Writer& writer, const char* theString);
Writer& operator<<(Writer& writer, char val);
Writer& operator<<(Writer& writer, double val);
Writer& operator<<(Writer& writer, long val);
Writer& operator<<(Writer& writer, unsigned long val);
Writer& operator<<(Writer& writer, bool val);
Writer& operator<<(Writer& writer, int val);

#ifndef ARDUINO
#include <string>
Writer& operator<<(Writer& writer, std::string val);
#endif

#endif
