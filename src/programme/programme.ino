#include "axis.h"
#include "setup.h"
#include "common.h"

// CONSTANTS
#define SLOW_SPEED_DELAY 2000
#define FAST_SPEED_DELAY 100

using namespace std;

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
    ArduinoProgram() : Program(m_writer) {
    }

    Writer& getWriter() {
      return m_writer;
    }
    unsigned long getCurrentTime() {
      return micros();
    }
    void sleepMs(int time) {
      delay(time); // FIXME: Should be sleep, not busy wait..
    }
    bool inputAvailable() {
      return Serial.available() > 0;
    }
    bool getInput(char* buf, int size) {
      Serial.readBytes(buf, size);
      return true; // FIXME: Is it always true?
    }

    ArduinoWriter m_writer;
};

ArduinoProgram p;

char input[256];

int inputCursor = 0;
byte inputSize = 0;

void setup() {

  //Initiate Serial communication.
  Serial.begin(115200);
  Serial.println("Setup...");

  setupAxes(&p.getWriter(), p.axes);
  
  Serial.println("Done");
}

#define MESSAGE_RECEIVED "ok"
#define MESSAGE_DONE "done"

void loop() {
  
  myLoop(p);
}
