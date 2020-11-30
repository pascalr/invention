#include "hx711.h"
#include "arduino_lib.h"

// TODO: H: Referencing
// TODO: +: Move forward
// TODO: -: Move backward

#define GRIP_PIN_DIR 8
#define GRIP_PIN_PWM 9
#define GRIP_RELEASE_STRENGTH 40 // From 0 to 255, 255 being 100% motor strength.
#define GRIP_REVERSE_DIR true

#define TURN_PIN_DIR 7
#define TURN_PIN_PWM 6
#define TURN_REVERSE_DIR false

long offset;

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

//void shake(Program& p, Motor* motor) {
//  
//  if (!motor) return;
//
//  unsigned long startTimeUs = p.getCurrentTime();
//  
//  unsigned long time = 0;
//  //unsigned long shakingTimeUs = 1000 * 1000; // WARNING: THIS DOES NOT WORK
//  //unsigned long shakingTimeUs = (unsigned long)1000 * (unsigned long)1000; // BUT THIS DOES
//  unsigned long shakingTimeUs = 2000000;
//
//  unsigned long lastShakeTime = 0;
//  unsigned long shakingIntervalUs = 100000;
//
//  bool forward = true;
//
//  while (time < shakingTimeUs) {
//    
//    if (askedToStop(p)) {
//      motor->stop();
//      break;
//    }
//
//    if (timeDifference(lastShakeTime, time) > shakingIntervalUs) {
//      //p.sleepMs(50); // Sleep a little so it is not too much of a big change to not loose steps.
//      motor->setMotorDirection(!motor->getMotorDirection());
//      lastShakeTime = time;
//    }
//
//    unsigned long now = p.getCurrentTime();
//    motor->run(now, SHAKE_SPEED_RPM);
//    time = timeDifference(startTimeUs, now);
//  }
//
//  motor->stop();
//}

#define BUF_SIZE 52
char buf[BUF_SIZE];

StepperConfig stepper_j;

void setup() {

  Serial.begin(115200);

  stepper_h.pin_dir = 2;
  stepper_h.pin_step = 3;
  stepper_h.pin_enable = 8;
  stepper_h.reverse_motor_direction = false;
  stepper_h.steps_per_unit = 200 * 2 * 16 / (360*12/61);
  stepper_h.min_delay = 500;
  stepper_h.max_delay = 10000;
  stepper_h.nominal_delay = 5000;

  //stepper_v.pin_dir = 2;
  //stepper_v.pin_step = 3;
  //stepper_v.pin_enable = 8;
  //stepper_v.reverse_motor_direction = false;
  //stepper_v.steps_per_unit = 200 * 2 * 16 / (360*12/61);
  //stepper_v.min_delay = 500;
  //stepper_v.max_delay = 10000;
  //stepper_v.nominal_delay = 5000;

  //stepper_t.pin_dir = 2;
  //stepper_t.pin_step = 3;
  //stepper_t.pin_enable = 8;
  //stepper_t.reverse_motor_direction = false;
  //stepper_t.steps_per_unit = 200 * 2 * 16 / (360*12/61);
  //stepper_t.min_delay = 500;
  //stepper_t.max_delay = 10000;
  //stepper_t.nominal_delay = 5000;

  //stepper_a.pin_dir = 2;
  //stepper_a.pin_step = 3;
  //stepper_a.pin_enable = 8;
  //stepper_a.reverse_motor_direction = false;
  //stepper_a.steps_per_unit = 200 * 2 * 16 / (360*12/61);
  //stepper_a.min_delay = 500;
  //stepper_a.max_delay = 10000;
  //stepper_a.nominal_delay = 5000;

  pinMode(stepper_j.pin_dir, OUTPUT);
  pinMode(stepper_j.pin_step, OUTPUT);
  
  //pinMode(LED_BUILTIN, OUTPUT);

  //calibrateEmpty();
};

void loop() {

  if (Serial.inputAvailable() > 0) {

    getInputLine(buf, BUF_SIZE);
    char cmd = buf[0];

    // ignore newline characters
    if (cmd == '\r' || cmd == '\n') return;

    Serial.print("echo: ");
    Serial.println(cmd);
    
    double nb;
    char* input = buf; input++;

    if (cmd == '#') {
      Serial.println("#: mobile");

    } else if (cmd == 'h') { // Print the help
      Serial.println("#: version");
      Serial.println("mj(relative destination): move the stepper j to a relative destination");
      Serial.println("f: free/release the gripper");
      Serial.println("g(strengh): grab with a given strengh from 0 to 255");
      Serial.println("s: stop everything! and send back when it was stopped at");
    
    } else if (cmd == 'm') { // Move

      char axis = *input;
      input++;
      if (parseNumber(&input, nb) < 0) {
        Serial.print("error: ");
        Serial.println("Invalid move destination. Not a number.");
        return;
      }

      if (axis == 'h') { // FIXME: Hardcoded axis name
        moveConstantSpeed(stepper_h, nb);
      //} else if (axis == 'v') {
      //  moveConstantSpeed(stepper_v, nb);
      //} else if (axis == 't') {
      //  moveConstantSpeed(stepper_t, nb);
      //} else if (axis == 'a') {
      //  moveConstantSpeed(stepper_a, nb);
      } else {
        Serial.print("error: ");
        Serial.println("Invalid axis name given.");
      }
    
    } else if (cmd == 'f') { // Release
      release();
    
    //} else if (cmd == 'k') { // Shake
    //  release();

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
