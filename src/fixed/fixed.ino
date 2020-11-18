#include "hx711.h"
#include "arduino_lib.h"

// https://github.com/aguegu/ardulibs/tree/master/hx711 

#define GRIP_PIN_DIR 8
#define GRIP_PIN_PWM 9
#define GRIP_RELEASE_STRENGTH 40 // From 0 to 255, 255 being 100% motor strength.

// If you need to reverse the motor direction, you can also just interchange the two motor wires.
#define GRIP_REVERSE_DIR false

// Hx711.DOUT - pin #A1
// Hx711.SCK - pin #A0
Hx711 scale(A1, A0);
  
ArduinoReader reader;
ArduinoWriter writer;

long offset;

void calibrateEmpty() {
  offset = scale.averageValue();
  scale.setOffset(offset);
}

void calibrateWithWeight(double weight) {
  float ratio = (scale.averageValue() - offset) / weight;
  Serial.print("ratio: ");
  Serial.println(ratio);
  scale.setScale(ratio);
}

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

    if (askedToStop(p)) {
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

void grab(double strength) {
  digitalWrite(GRIP_PIN_DIR, GRIP_REVERSE_DIR ? LOW : HIGH);
  analogWrite(GRIP_PIN_PWM, strength);
}

void release() {
  digitalWrite(GRIP_PIN_DIR, GRIP_REVERSE_DIR ? HIGH : LOW);
  analogWrite(GRIP_PIN_PWM, GRIP_RELEASE_STRENGTH);
}

#define BUF_SIZE 52
char buf[BUF_SIZE];

StepperConfig stepper_j;

void setup() {

  Serial.begin(115200);


  //p.axisT.microsteps = 16;
  p.axisT.setStepsPerUnit(200 * 2 * 16 / (360*12/61));
  p.axisT.steps_per_turn = 200 * 2 * 16;
  //p.axisT.limitSwitchPin = 12;
  p.axisT.setupPins(8,2,3);
  p.axisT.setReverseMotorDirection(true);
  p.axisT.percent_p = 0.4;
  p.axisT.min_delay = 100;
  p.axisT.max_delay = 2000;

  stepper_j.pin_dir = ;
  stepper_j.pin_step = ;
  stepper_j.pin_enable = ;
  stepper_j.reverse_motor_direction = ;
  stepper_j.steps_per_unit = 200 * 2 * 16 / (360*12/61);
  stepper_j.min_delay = 500;
  stepper_j.max_delay = 10000;
  stepper_j.nominal_delay = 5000;
};

  //pinMode(LED_BUILTIN, OUTPUT);

  //calibrateEmpty();
}

void loop() {

  if (reader.inputAvailable()) {

    getInputLine(reader, buf, BUF_SIZE);
    char cmd = buf[0];

    // ignore newline characters
    if (cmd == '\r' || cmd == '\n') return;

    Serial.print("echo: ");
    Serial.println(cmd);
    
    double nb;
    char* input = buf; input++;

    if (cmd == 'w') { // Get weight
      Serial.print("weight: ");
      Serial.println(scale.getGram(), 1); // Print the gram value with one decimal precision
      
    } else if (cmd == 'c') { // Calibrate with a weight 
      if (parseNumber(&input, nb) < 0) {
        Serial.print("error: ");
        Serial.println("Invalid number given.");
        return;
      }
      calibrateWithWeight(nb);
    
    } else if (cmd == '#') { // Print the version
      Serial.println("#: fixed");
    
    } else if (cmd == 't') { // Set the ratio
      if (parseNumber(&input, nb) < 0) {
        Serial.print("error: ");
        Serial.println("Invalid number given.");
        return;
      }
      scale.setScale(nb);

    } else if (cmd == 'e') { // Calibrate empty
      calibrateEmpty();

    } else if (cmd == 'm') { // Move

      if (*input == 'j') { // FIXME: Hardcoded axis name
        input++;
        if (parseNumber(&input, nb) < 0) {
          Serial.print("error: ");
          Serial.println("Invalid move destination. Not a number.");
          return;
        }
        moveConstantSpeed(stepper_j, nb);
      } else {
        Serial.print("error: ");
        Serial.println("Invalid axis name given.");
      }
    
    } else if (cmd == 'f') { // Release
      release();
    
    } else if (cmd == 'g') { // Grab
      if (parseNumber(&input, nb) < 0) {
        Serial.print("error: ");
        Serial.println("Invalid number given.");
        return;
      }
      grab(nb);
    
    } else {
      Serial.print("error: ");
      Serial.println("Unkown command");
      return;
    }
    Serial.println("done");
  }

  delay(100); // OPTIMIZE: Probably useless since it is busy wait I believe

}
