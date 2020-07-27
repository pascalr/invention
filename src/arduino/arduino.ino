#include "utils.h"
#include "axis.h"
#include "setup.h"
#include "program.h"
#include "input_parser.h"

#include "MeDCMotor.h"

MeDCMotor motor1(8,9);

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
    
    void sleepMs(int time) {
      delay(time); // FIXME: Should be sleep, not busy wait..
    }
    bool inputAvailable() {
      return Serial.available() > 0;
    }
    int getByte() {
      return Serial.read();
    }
    bool getInput(char* buf, int size) {
      size_t inputSize = Serial.readBytes(buf, size);
      if (inputSize == 0) return false;
      
      buf[inputSize] = 0;
      rtrim(buf);
      return true;
    }

    unsigned long getCurrentTime() {
      return micros();
    }

  protected:

    ArduinoWriter m_writer;
};

ArduinoProgram p;

unsigned long lastPrint = 0;
int count = 0;
bool wasHigh = false;

void setup() {

  //Initiate Serial communication.
  Serial.begin(115200);
  Serial.println("Setup...");

  setupAxes(p);
  
  pinMode(LED_BUILTIN, OUTPUT);

  wasHigh = analogRead(5) > 500;
  
  Serial.println(MESSAGE_READY);
}

void loop() {
  unsigned long time = millis();
  if (time - lastPrint > 1000) {
    Serial.print("RPM: ");
    // There are 8 steps per turn
    float rpm = count / 8.0 * 60;
    Serial.println(rpm);
    lastPrint = time;
    count = 0;
  }
  if (analogRead(5) > 500) {
    wasHigh = true;
  } else if (wasHigh) {
    wasHigh = false;
    count += 1;
  }
  //delay(500);
  //if (analogRead(5) > 500) {
  //  digitalWrite(LED_BUILTIN, HIGH);
  //} else {
  //  digitalWrite(LED_BUILTIN, LOW);
  //}
  //motor1.run(50);
  //myLoop(p);
}
