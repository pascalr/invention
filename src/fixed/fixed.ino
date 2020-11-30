#include "hx711.h"
#include "arduino_lib.h"

// https://github.com/aguegu/ardulibs/tree/master/hx711 

#define GRIP_PIN_DIR 8
#define GRIP_PIN_PWM 9
#define GRIP_RELEASE_STRENGTH 40 // From 0 to 255, 255 being 100% motor strength.
#define GRIP_REVERSE_DIR true

#define TURN_PIN_DIR 7
#define TURN_PIN_PWM 6
#define TURN_REVERSE_DIR false

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

void grab(double strength) {
  digitalWrite(GRIP_PIN_DIR, GRIP_REVERSE_DIR ? LOW : HIGH);
  analogWrite(GRIP_PIN_PWM, strength);
}

void release() {
  digitalWrite(GRIP_PIN_DIR, GRIP_REVERSE_DIR ? HIGH : LOW);
  analogWrite(GRIP_PIN_PWM, GRIP_RELEASE_STRENGTH);
}

void turn(double strength) {
  digitalWrite(TURN_PIN_DIR, TURN_REVERSE_DIR ? HIGH : LOW);
  analogWrite(TURN_PIN_PWM, strength);
}

#define BUF_SIZE 52
char buf[BUF_SIZE];

StepperConfig stepper_j;

void setup() {

  Serial.begin(115200);

  stepper_j.pin_dir = 2;
  stepper_j.pin_step = 3;
  stepper_j.pin_enable = 8;
  stepper_j.reverse_motor_direction = false;
  stepper_j.steps_per_unit = 200 * 2 * 16 / (360*12/61);
  stepper_j.min_delay = 500;
  stepper_j.max_delay = 10000;
  stepper_j.nominal_delay = 5000;

  pinMode(stepper_j.pin_dir, OUTPUT);
  pinMode(stepper_j.pin_step, OUTPUT);
  
  //pinMode(LED_BUILTIN, OUTPUT);

  //calibrateEmpty();
};

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

    } else if (cmd == 'h') { // Print the help
      Serial.println("#: version");
      Serial.println("w: get weight");
      Serial.println("c(float): calibrate with a given weight on the scale");
      Serial.println("t: set the ratio of the scale (the value given by the command c)");
      Serial.println("e: calibrate empty");
      Serial.println("mj(relative destination): move the stepper j to a relative destination");
      Serial.println("f: free/release the gripper");
      Serial.println("r: not used anymore, for the other gripper...");
      Serial.println("g(strengh): grab with a given strengh from 0 to 255");
      Serial.println("s: stop everything! and send back when it was stopped at");
    
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

    } else if (cmd == 'r') { // Turn
      if (parseNumber(&input, nb) < 0) {
        Serial.print("error: ");
        Serial.println("Invalid number given.");
        return;
      }
      turn(nb);
    
    } else if (cmd == 'g') { // Grab
      if (parseNumber(&input, nb) < 0) {
        Serial.print("error: ");
        Serial.println("Invalid number given.");
        return;
      }
      grab(nb);

    } else if (cmd == 's' || cmd == 'S') {
      analogWrite(GRIP_PIN_PWM, 0.0);
      analogWrite(TURN_PIN_PWM, 0.0);
    
    } else {
      Serial.print("error: ");
      Serial.println("Unkown command");
      return;
    }
    Serial.println("done");
  }

  delay(100); // OPTIMIZE: Probably useless since it is busy wait I believe

}
