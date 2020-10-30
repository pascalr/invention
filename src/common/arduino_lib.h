//#include "utils.h"
#include "../core/reader/reader.h"
#include "../core/writer/writer.h"

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
