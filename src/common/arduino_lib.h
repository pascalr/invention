//#include "utils.h"
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

//void sleepMs(int time) {
//  delay(time); // FIXME: Should be sleep, not busy wait..
//}
//
//unsigned long getCurrentTimeUs() {
//  return micros();
//}
  
//Serial.begin(115200);

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

void loop() {
  //Serial.println(analogRead(4));
  //delay(1000);

  //if (timeDifference(lastPrintUs, micros()) > printIntervalUs) {
  //  Serial.println("-----");
  //  Serial.println(timeDifference(p.axisT.m_start_time, micros()));
  //  Serial.println(p.axisT.next_step_delay);
  //  Serial.println(p.axisT.next_step_time);
  //  lastPrintUs = micros();
  //}

  myLoop(p);
}
