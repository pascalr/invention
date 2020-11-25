//#include "utils.h"
#include "../core/reader/reader.h"
#include "../core/writer/writer.h"

class StepperConfig {
  public:
    int pin_dir;
    int pin_step;
    int pin_enable;

    bool reverse_motor_direction;

    double steps_per_unit;

    int min_delay = 500;
    int max_delay = 10000;

    int nominal_delay = 5000; // Used for constant speed
};

// Returns the number of bytes read.
// Arduino already has a function that does this, but it was so slow I don't know why.
int getInputLine(Reader& reader, char* buf, int bufSize) {
  int i = 0;
  while (true) {
    if (reader.inputAvailable()) {

      char ch = reader.getByte();

      if (ch == '\n') break;
      if (i >= bufSize-1) break;
      buf[i] = ch;
      i++;
    }
    delay(50); // OPTIMIZE: Probably useless since it is busy wait I believe
  }
  buf[i] = '\0';
  return i;
}

int parseNumber(char** input, double& n) {
  char* pEnd;
  n = strtod(*input, &pEnd);
  // If no number was found
  if (pEnd == *input) {return -1;}
  *input = pEnd;
  return 0;
}

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
