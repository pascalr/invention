#include "program.h"
#include "serialize.h"

int parseNumber(Program& p, double& n) {

  char number[MAX_NUMBER_CHAR + 1];
  bool periodFound = false;

  int i = 0;
  for(; i < MAX_NUMBER_CHAR; i++) {
    char c = p.getChar();

    // ignore whitespaces
    if (c == ' ') {
      i--;

    // digits are always ok
    } else if (c >= '0' && c <= '9') {
   
    // sign at beginning is ok 
    } else if (i == 0 && (c == '-' || c == '+')) {

    // period found
    } else if (!periodFound && c == '.') {
      periodFound = true;
    
    // end of number
    } else {
      break;
    }
    number[i] = c;
  }
  if (i == 0) {
    return -1;
  }
  number[i+1] = '\0';
  n = atof(number);
  return 1;
}

Axis* parseInputAxis(Program& p, Axis* &axis) {
  char name = p.getChar();
  axis = axisByLetter(p.axes, name);
  return axis;
}

int parseActionCommand(char cmd, Program& p) {

  Axis* axis;
  double number;

  // Prepare
  for (int i = 0; p.axes[i] != NULL; i++) {
    p.axes[i]->prepare(p.getCurrentTime());
  }

  // Move
  if (cmd == 'M' || cmd == 'm') {
    if (!parseInputAxis(p, axis)) {return ERROR_EXPECTED_AXIS;}
    if (parseNumber(p,number) < 0) {return ERROR_EXPECTED_NUMBER;}
    axis->setDestination(number);
    axis->setMotorEnabled(true);

  // Home (referencing) (currently only supports referencing all (not HX or HY)
  } else if (cmd == 'H' || cmd == 'h') {
    p.getWriter() << "Referencing...\n";
    for (int i = 0; p.axes[i] != NULL; i++) {
      p.axes[i]->startReferencing();
    }

  // Wait or sleep for some time
  } else if (cmd == 'w' || cmd == 'W') {
    if (parseNumber(p,number) < 0) {return ERROR_EXPECTED_NUMBER;}
    p.sleepMs(number);

  // Move forward
  } else if (cmd == '+') {
    if (!parseInputAxis(p, axis)) {return ERROR_EXPECTED_AXIS;}
    axis->rotate(FORWARD);

  // Move backward
  } else if (cmd == '-') {
    if (!parseInputAxis(p, axis)) {return ERROR_EXPECTED_AXIS;}
    axis->rotate(REVERSE);
  }

  p.getWriter() << "Cmd: " << cmd << "\n";

  // After input
  for (int i = 0; p.axes[i] != NULL; i++) {
    p.axes[i]->afterInput();
  }

  return 0;
}

void myLoop(Program& p) {
  if (p.inputAvailable()) {
    int incomingByte = p.getByte();
    if (incomingByte < 0) {
      return;
    }
    char cmd = (char) incomingByte;

    // stop
    if (cmd == 's' || cmd == 'S') {
      for (int i = 0; p.axes[i] != NULL; i++) {
        p.axes[i]->stop();
      }
      p.getWriter() << "Stopped\n";
      p.isWorking = false; // Maybe not necessary because already told the axes to stop. Anyway it does not hurt..

    // info
    } else if (cmd == '?') { // info
      p.getWriter() << "\n" << MESSAGE_JSON << "\n";
      serialize<Writer>(p, p.getWriter());
      p.getWriter() << "\n";

    // position (faster than info)
    } else if (cmd == '@') { // TODO
      
    // ignore
    } else if (cmd == '\r' || cmd == '\n') {
    
    // others are action command, so the program should not be working already..
    } else if (p.isWorking) {
      p.getWriter() << "Error, received an action command when previous was not over. Cmd = " << cmd << '\n';
    } else {
      p.isWorking = true;
      int code = parseActionCommand(cmd, p);
      if (code < 0) {
        p.getWriter() << "Exception: Code: " << code << '\n';
      }
      return;
    }
  }
  if (!p.isWorking) {
    p.sleepMs(10);
    return;
  }

  bool stillWorking = false;
  for (int i = 0; p.axes[i] != NULL; i++) {
    stillWorking = stillWorking || p.axes[i]->handleAxis(p.getCurrentTime());
  }

  if (!stillWorking) {
    p.isWorking = false;
    p.getWriter() << MESSAGE_DONE << '\n';
  }
}
