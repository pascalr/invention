#include "program.h"
#include "serialize.h"

#include <stdlib.h>

int parseNumber(char** input, double& n) {
  char* pEnd;
  n = strtod(*input, &pEnd);
  // If no number was found
  if (pEnd == *input) {return -1;}
  *input = pEnd;
  return 0;
}

Motor* parseInputMotorAxis(Program& p, char** input, Motor* &axis) {
  char name = **input;

  for (int i = 0; p.motors[i] != 0; i++) {
    if (toupper(name) == p.motors[i]->getName()) {
      axis = p.motors[i];
      (*input)++;
      return axis;
    }
  }

  return 0;
}

StepperMotor* parseStepper(Program& p, char** input) {
  char name = **input;

  for (int i = 0; p.steppers[i] != 0; i++) {
    if (toupper(name) == p.steppers[i]->getName()) {
      (*input)++;
      return p.motors[i];
    }
  }

  return 0;
}

bool askedToStop(Program& p) {
  if (p.getReader().inputAvailable()) {
    int incomingByte = p.getReader().getByte();
    if (incomingByte < 0) {
      return;
    }
    char cmd = (char) incomingByte;

    // stop
    if (cmd == 's' || cmd == 'S') {
      p.stopMoving();
  }
}

// TODO: Do referencing the same way as this. Much simpler.
//
// Faster way to move a stepper motor allowing for higher micro-stepping
// Only checks if there is input available after each step
// OPTIMIZE: I can do even better than this by not using micros() which only
// has an accuracy of 8 microseconds
// OPTIMIZE: Use interrupts instead of askedToStop I believe
void moveStepper(Program& p, StepperMotor* motor, double destination) {

  if (!motor) return;

  unsigned long startTime = p.getCurrentTime();
  unsigned long timeSinceStart = 0;
  motor->setDestination(destination);

  while (!motor->isDestinationReached()) {

    if (askedToStop(p)) {
      motor->stop();
      break;
    }
    motor->turnOneStep(timeSinceStart);
    delayMicroseconds(motor->next_step_delay);
    timeSinceStart = timeDifference(startTime, p.getCurrentTime());
  }
}

int parseActionCommand(char cmd, Program& p) {

  p.getWriter() << "Cmd: " << cmd << "\n";

  Motor* motorAxis;
  double number;
  int status;

  // Prepare
  unsigned long time = p.getCurrentTime();
  for (int i = 0; p.motors[i] != 0; i++) {
    p.motors[i]->prepareWorking(time);
  }

  char* input = p.getInputLine();

  // Move
  if (cmd == 'M' || cmd == 'm') {
    if (!parseInputMotorAxis(p, &input, motorAxis)) {return ERROR_EXPECTED_AXIS;}
    if (parseNumber(&input,number) < 0) {return ERROR_EXPECTED_NUMBER;}
    if ((status = motorAxis->getto(number)) < 0) {return status;}
  
  } else if (cmd == 'd' || cmd == 'D') { // move only one stepper (faster because not checking all axes)
    StepperMotor* stepper;
    if (!(stepper = parseStepper(p, &input))) {return ERROR_EXPECTED_AXIS;}
    if (parseNumber(&input,number) < 0) {return ERROR_EXPECTED_NUMBER;}
    moveStepper(p, stepper, number);

  // Home (referencing) (currently only supports referencing all (not HX or HY)
  } else if (cmd == 'H' || cmd == 'h') {
    if (!parseInputMotorAxis(p, &input, motorAxis)) {return ERROR_EXPECTED_AXIS;}
#ifdef ARDUINO
    motorAxis->startReferencing();
#else
    if (motorAxis->getName() != p.axisR.getName()) {motorAxis->startReferencing();}
#endif

  // Wait or sleep for some time
  } else if (cmd == 'w' || cmd == 'W') {
    if (parseNumber(&input,number) < 0) {return ERROR_EXPECTED_NUMBER;}
    p.sleepMs(number);

  // Release
  } else if (cmd == 'r' || cmd == 'R') {
#ifdef ARDUINO
    p.axisR.release();
#endif

  // Grab
  } else if (cmd == 'g' || cmd == 'G') {
    if (parseNumber(&input,number) < 0) {return ERROR_EXPECTED_NUMBER;}
#ifdef ARDUINO
    p.axisR.grab(number);
#endif

  // Move forward
  } else if (cmd == '+') {
    if (!parseInputMotorAxis(p, &input, motorAxis)) {return ERROR_EXPECTED_AXIS;}
    motorAxis->rotate(FORWARD);

  // Move backward
  } else if (cmd == '-') {
    if (!parseInputMotorAxis(p, &input, motorAxis)) {return ERROR_EXPECTED_AXIS;}
    motorAxis->rotate(REVERSE);
  }

  return 0;
}

void myLoop(Program& p) {
  if (p.getReader().inputAvailable()) {
    int incomingByte = p.getReader().getByte();
    if (incomingByte < 0) {
      return;
    }
    char cmd = (char) incomingByte;

    // stop
    if (cmd == 's' || cmd == 'S') {
      p.stopMoving();

    // info
    } else if (cmd == '?') { // info
      p.getWriter() << "\nTime: " << p.getCurrentTime();
      p.getWriter() << "\n" << MESSAGE_JSON << "\n";
      serialize<Writer>(p, p.getWriter());
      p.getWriter() << "\n";

    // position (faster than info)
    } else if (cmd == '@') {
      p.getWriter() << "\n" << MESSAGE_POSITION << "\n";
      p.getWriter() << p.axisH.getPosition() << " "
                    << p.axisV.getPosition() << " "
                    << p.axisT.getPosition() << "\n";
      
    // ignore
    } else if (cmd == '\r' || cmd == '\n') {
    
    // others are action command, so the program should not be working already..
    } else if (p.isWorking) {
      p.getWriter() << "Error, received an action command when previous was not over. Cmd = " << cmd << '\n';
    } else {
      p.isWorking = true;
      int code = parseActionCommand(cmd, p);
      if (code < 0) {
        p.getWriter() << "Exception: Code: " << code << ".\n";
        /*while (true) { Doesnt work yet because parseActionCommand might have read the \n already
          char throwAway = p.getChar();
          if (throwAway == '\n') {break;}
          p.getWriter() << throwAway;
        }
        p.getWriter() << ".\n";*/
      }
      return;
    }
  }
  if (!p.isWorking) {
    p.sleepMs(10);
    return;
  }

  bool stillWorking = false;
  for (int i = 0; p.motors[i] != 0; i++) {
    bool isWorking = p.motors[i]->handleAxis(p.getCurrentTime());
    stillWorking = isWorking || stillWorking;
  }

  if (!stillWorking) {
    p.isWorking = false;
    p.getWriter() << MESSAGE_DONE << '\n';
  }
}
