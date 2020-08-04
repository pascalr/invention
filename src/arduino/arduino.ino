#include "utils.h"
#include "axis.h"
#include "setup.h"
#include "program.h"
#include "input_parser.h"
#include "reader/reader.h"

class ArduinoReader : public Reader {

  public:
    
    bool inputAvailable() {
      return Serial.available() > 0;
    }
    int getByte() {
      return Serial.read();
    }
};

class ArduinoWriter : public Writer {
  public:
    void doPinMode(int pin, bool type) {
      pinMode(pin, type);
    }
    void doDigitalWrite(int pin, bool value) {
      digitalWrite(pin, value);
    }
    double doDigitalRead(int pin) {
      return digitalRead(pin);
    }

    void doPrint(const char* theString) {
      Serial.print(theString);
    }

    void doPrint(char val) {
      Serial.print(val);
    }

    void doPrint(double val) {
      Serial.print(val);
    }

    void doPrint(long val) {
      Serial.print(val);
    }

    void doPrint(unsigned long val) {
      Serial.print(val);
    }

    void doPrint(bool val) {
      Serial.print(val);
    }

    void doPrint(int val) {
      Serial.print(val);
    }

};

class ArduinoProgram : public Program {
  public:
    ArduinoProgram() : Program(m_writer, m_reader) {
    }

    Writer& getWriter() {
      return m_writer;
    }

    Reader& getReader() {
      return m_reader;
    }
    
    void sleepMs(int time) {
      delay(time); // FIXME: Should be sleep, not busy wait..
    }

    unsigned long getCurrentTime() {
      return micros();
    }

  protected:

    ArduinoWriter m_writer;
    ArduinoReader m_reader;
};

ArduinoProgram p;

void setup() {

  //Initiate Serial communication.
  Serial.begin(115200);
  Serial.println("Setup...");

  setupAxes(p);
  
  pinMode(LED_BUILTIN, OUTPUT);

  Serial.println(MESSAGE_READY);
}

void loop() {
  myLoop(p);
}
