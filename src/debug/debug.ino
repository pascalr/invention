#include "utils.h"
#include "program.h"
#include "input_parser.h"
#include "reader/reader.h"
#include "writer/writer.h"

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

unsigned long printIntervalUs = 0.1 * 1000 * 1000;
unsigned long lastPrintUs = 0;
bool isLightOn = false;

void debugSwitch(int pin) {
  bool lightMustBeOn = digitalRead(pin) == HIGH;
  digitalWrite(LED_BUILTIN, lightMustBeOn ? HIGH : LOW);
  if (lightMustBeOn) {
    Serial.println("Switch activated!");
    delay(200);
  }
  delay(10);
}

void debugDigital(int pin) {
  bool value = digitalRead(pin) == LOW;
  digitalWrite(LED_BUILTIN, value ? HIGH : LOW);

  if (value) {
    Serial.println("0V!");
  }

  //if (timeDifference(lastPrintUs, micros()) > printIntervalUs) {
  //  Serial.println(value);
  //  lastPrintUs = micros();
  //}
}

// 1023 is the max value of analogRead
void debugAnalog(int pin) {
  int value = analogRead(pin);
  bool lightMustBeOn = value > 500;
  digitalWrite(LED_BUILTIN, lightMustBeOn ? HIGH : LOW);

  if (value > 500) {
    Serial.println(value);
  }

  if (timeDifference(lastPrintUs, micros()) > printIntervalUs) {
    Serial.println(value);
    lastPrintUs = micros();
  }

  delay(50);
}

void debugEncoder(int pin) {
  bool lightMustBeOn = analogRead(pin) > 500;
  digitalWrite(LED_BUILTIN, lightMustBeOn ? HIGH : LOW);
  delay(10);
}

void testing1212() {
  digitalWrite(LED_BUILTIN, HIGH);
}

void debugGeneral() {
  //Serial.println(analogRead(4));
  //delay(1000);

  //if (timeDifference(lastPrintUs, micros()) > printIntervalUs) {
  //  Serial.println("-----");
  //  Serial.println(timeDifference(p.axisT.m_start_time, micros()));
  //  Serial.println(p.axisT.next_step_delay);
  //  Serial.println(p.axisT.next_step_time);
  //  lastPrintUs = micros();
  //}

  isLightOn = !isLightOn;
  digitalWrite(LED_BUILTIN, isLightOn ? HIGH : LOW);

  Serial.println(analogRead(4));
  
  delay(500);
  
}

void loop() {

  //testing1212();
  //debugEncoder(5);
  //debugSwitch(12);
  //debugAnalog(4);
  debugDigital(12);
  //myLoop(p);
  //debugGeneral();
}
