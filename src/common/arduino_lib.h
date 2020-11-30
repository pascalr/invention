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

bool askedToStop() {
  while (Serial.available() > 0) {
    int incomingByte = Serial.read();
    if (incomingByte < 0) {return false;}
    char cmd = (char) incomingByte;

    // stop
    if (cmd == 's' || cmd == 'S') {
      return true;
    }
  }
  return false;
}

// OPTIMIZE: Use interrupts instead of askedToStop I believe
// Relative movement. Move from zero to destination.
void moveConstantSpeed(StepperConfig stepper, double destination) {

  bool dir = destination > 0 ? LOW : HIGH;
  dir = stepper.reverse_motor_direction ? !dir : dir;
  digitalWrite(stepper.pin_dir, dir);

  bool isStepHigh = digitalRead(stepper.pin_step);

  for (unsigned long pos = 0; pos < abs(destination) * stepper.steps_per_unit; pos++) {

    if (askedToStop()) {
      // TODO: When stopped, write at what position.
      break;
    }

    digitalWrite(stepper.pin_step, isStepHigh ? LOW : HIGH);
    isStepHigh = !isStepHigh;
    delayMicroseconds(stepper.nominal_delay);
  }
}

//// OPTIMIZE: Use interrupts instead of askedToStop I believe
//// Relative movement. Move from zero to destination.
//// TODO: When stopped, write at what position.
//// Using percent per time, with a trapezoidal curve
//void move(StepperConfig stepper, double destination) {
//
//  unsigned long startTime = micros();
//  unsigned long timeSinceStart = 0;
//
//  bool dir = destination > 0 ? LOW : HIGH;
//  dir = stepper.reverse_motor_direction ? !dir : dir;
//  digitalWrite(stepper.pin_dir, dir);
//
//  double min_percent = (double) min_delay / (double) max_delay;
//  double percent = min_percent; // Start at the minimum percentage of max speed
//  int phaseNb = 1; // what phase the motor is in
//
//  bool isStepHigh = digitalRead(stepper.pin_step);
//
//  double max_percent_reached = 0;
//  double timeSinceStartS = (timeSinceStart - lostTime) / 1000000.0;
//  double time_started_decelerating = 0;
//      
//  //unsigned long dest = abs(destination) * stepper.steps_per_unit; // destination in steps
//  //unsigned long pos = 0; // position in steps
//  //while (pos < dest) {
//  //pos++;
//  for (unsigned long pos = 0; pos < abs(destination) * stepper.steps_per_unit; pos++) {
//
//    if (askedToStop(p)) {
//      motor->stop(); break;
//    }
//
//    digitalWrite(stepper.pin_step, isStepHigh ? LOW : HIGH);
//    isStepHigh = !isStepHigh;
//    lost_time = timeSinceStart - next_step_time;
//
//    // Accelerating
//    if (phaseNb == 1) {
//
//      if (pos >= dest / 2.0) {
//        phaseNb = 3;
//        time_started_decelerating = timeSinceStartS;
//        debug_time_finished_accelerating_s = timeSinceStartS;
//        max_percent_reached = percent;
//
//      } else {
//        percent = percent_p * timeSinceStartS;
//        if (percent >= 1.0) {
//          distance_accelerating = distanceTravelledSteps;
//          max_percent_reached = 1.0;
//          percent = 1.0;
//          phaseNb = 2;
//        }
//      }
//
//    // Constant speed
//    } else if (phaseNb == 2) {
//
//      if (pos > dest - distance_accelerating) {
//        phaseNb = 3;
//        time_started_decelerating = timeSinceStartS;
//        debug_time_finished_accelerating_s = timeSinceStartS;
//      }
//
//    // Decelerating
//    } else {
//
//      percent = max_percent_reached - percent_p * (timeSinceStartS - time_started_decelerating);
//
//    }
//
//    next_step_delay = (percent <= min_percent) ? max_delay : min_delay / percent;
//    
//    next_step_time = timeSinceStart + next_step_delay;// - lost_time;
//
//    delayMicroseconds(motor->next_step_delay);
//    timeSinceStart = timeDifference(startTime, p.getCurrentTime());
//  }
//}

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

// Returns the number of bytes read.
// Arduino already has a function that does this, but it was so slow I don't know why.
int getInputLine(char* buf, int bufSize) {
  int i = 0;
  while (true) {
    if (Serial.inputAvailable() > 0) {

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
