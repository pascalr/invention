#ifndef WRITER_H
#define WRITER_H

// FIXME: Make the writer class simply a child of ostream
class Writer {
  public:
    virtual void doPinMode(int pin, bool type) = 0;
    virtual void doDigitalWrite(int pin, bool value) = 0;
    virtual double doDigitalRead(int pin) = 0;
    virtual void doPrint(const char* val) = 0;
    virtual void doPrint(char val) = 0;
    virtual void doPrint(double val) = 0;
    virtual void doPrint(long val) = 0;
    virtual void doPrint(int val) = 0;
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

#endif
