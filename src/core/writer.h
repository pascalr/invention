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

Writer& operator<<(Writer& writer, const char* theString) {
  writer.doPrint(theString);
  return writer;
}

Writer& operator<<(Writer& writer, char val) {
  writer.doPrint(val);
  return writer;
}

Writer& operator<<(Writer& writer, double val) {
  writer.doPrint(val);
  return writer;
}

Writer& operator<<(Writer& writer, long val) {
  writer.doPrint(val);
  return writer;
}

Writer& operator<<(Writer& writer, unsigned long val) {
  writer.doPrint(val);
  return writer;
}

Writer& operator<<(Writer& writer, bool val) {
  writer.doPrint(val);
  return writer;
}

Writer& operator<<(Writer& writer, int val) {
  writer.doPrint(val);
  return writer;
}

template <typename T>
void writeJson(T& writer, const char* key, const char* val) {
  writer << "\"" << key << "\": \"" << val << "\", ";
}
template <typename T>
void writeJson(T& writer, const char* key, bool val) {
  writer << "\"" << key << "\": " << (val ? "true" : "false") << ", ";
}
template <typename T>
void writeJson(T& writer, const char* key, char val) {
  writer << "\"" << key << "\": \"" << val << "\", ";
}
template <typename T>
void writeJson(T& writer, const char* key, double val) {
  writer << "\"" << key << "\": " << val << ", ";
}
template <typename T>
void writeJson(T& writer, const char* key, int val) {
  writer << "\"" << key << "\": " << val << ", ";
}
template <typename T>
void writeJson(T& writer, const char* key, long val) {
  writer << "\"" << key << "\": " << val << ", ";
}
template <typename T>
void writeJson(T& writer, const char* key, unsigned long val) {
  writer << "\"" << key << "\": " << val << ", ";
}

#endif
