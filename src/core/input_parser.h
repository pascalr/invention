#ifndef INPUT_PARSER_H
#define INPUT_PARSER_H

#include "program.h"
#include "serialize.h"

#include <exception>

class ParseInputException : public exception {};

class ExpectedNumberException : public ParseInputException {
  virtual const char* what() const throw() {
    return "An error occur while parsing the input. Expected a number.";
  }
};

class ExpectedAxisException : public ParseInputException {
  virtual const char* what() const throw() {
    return "An error occur while parsing the input. Expected an axis.";
  }
};

bool isNumberSymbol(char c) {
  return (c >= '0' && c <= '9') || c == '.';
}

int parseMove(Axis** axes, const char* cmd, int oldCursor) {
  int i;
  for (i = oldCursor; cmd[i] != '\0'; i++) {
    Axis* axis = axisByLetter(axes, cmd[i]);
    if (axis) {
      axis->setDestination(atof(cmd+i+1));
      axis->setMotorEnabled(true);
    } else if(!isNumberSymbol(cmd[i])) {
      return i; // End of move command, stop
    }
  }
  return i;
}

#define MAX_NUMBER_CHAR 12
double parseNumber(Program& p) {

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
    throw ExpectedNumberException();
  }
  number[i+1] = '\0';
  return atof(number);
}

Axis* parseInputAxis(Program& p) {
  char name = p.getChar();
  Axis* axis = axisByLetter(p.axes, name);
  if (!axis) {
    throw ExpectedAxisException();
  }
}

void parseInputCommand(char cmd, Program& p) {
  // Move
  if (cmd == 'M' || cmd == 'm') {
    Axis* axis = parseInputAxis(p);
    double destination = parseNumber(p);
    axis->setDestination(destination);

  // Home (referencing) (currently only supports referencing all (not HX or HY)
  } else if (cmd == 'H' || cmd == 'h') {
    p.getWriter() << "Referencing...\n";
    for (int i = 0; p.axes[i] != NULL; i++) {
      p.axes[i]->startReferencing();
    }

  // Wait or sleep for some time
  } else if (cmd == 'w' || cmd == 'W') {
    double waitTime = parseNumber(p);
    p.sleepMs(waitTime);

  // Move forward
  } else if (cmd == '+') {
    Axis* axis = parseInputAxis(p);
    axis->rotate(FORWARD);

  // Move backward
  } else if (cmd == '-') {
    Axis* axis = parseInputAxis(p);
    axis->rotate(REVERSE);
  }

  p.getWriter() << "Cmd: " << cmd << "\n";
}

int parseInput(const char* input, Program& p, int oldCursor) {

  // Should the cursor passed to the function be a pointer?
  int cursor = oldCursor + 1;
  int size = strlen(input);
  char cmd = input[oldCursor];
  
  if (cmd == 'M' || cmd == 'm') {
    cursor = parseMove(p.axes, input, cursor);
  } else if (cmd == 's' || cmd == 'S') { // stop
    for (int i = 0; p.axes[i] != NULL; i++) {
      p.axes[i]->stop();
    }
    cursor = size; // Disregard everything else after the stop command.
  } else if (cmd == 'H' || cmd == 'h') { // home reference (eg. H, or HX, or HY, ...)
    p.getWriter() << "Referencing...\n";
    if (size == 1) {
      for (int i = 0; p.axes[i] != NULL; i++) {
        p.axes[i]->startReferencing();
      }
    } else {
      Axis* axis = axisByLetter(p.axes, input[cursor]);
      cursor++;
      if (axis) {
        axis->startReferencing();
      }
      // TODO: Handle error
    }
  } else if (cmd == 'w' || cmd == 'W') { // wait or sleep for some time
    double waitTime = atof(input + cursor);
    while (isNumberSymbol(input[cursor])) {cursor++;}
    #ifndef DEBUG
      delay(waitTime);
    #endif
  } else if (cmd == '?') {
    p.getWriter() << "\n" << MESSAGE_JSON << "\n";
    serialize<Writer>(p, p.getWriter());
    p.getWriter() << "\n";
  } else if (cmd == '+') {
    Axis* axis = axisByLetter(p.axes, input[cursor]);
    cursor++;
    if (axis) {
      axis->rotate(FORWARD);
    }
    // TODO: Handle error
  } else if (cmd == '-') {
    Axis* axis = axisByLetter(p.axes, input[cursor]);
    cursor++;
    if (axis) {
      axis->rotate(REVERSE);
    }
    // TODO: Handle error
  }

  p.getWriter() << "Cmd: " << input+oldCursor << "\n";
  /*if (cursor < size) {
    char tmp = input[cursor];
    input[cursor] = '\0';
    writer->doPrintLn(input+oldCursor);
    input[cursor] = tmp;
  } else {
    writer->doPrintLn(input+oldCursor);
  }*/
  
  /*char sint[5];
  itoa(cmd, sint, 10);
  writer->doPrintLn(sint);*/

  return cursor;
}

void parseMoveByte() {
}

void parseActionCommand(char cmd, Program& p) {
  for (int i = 0; p.axes[i] != NULL; i++) {
    p.axes[i]->prepare(p.getCurrentTime());
  }

  // Move
  if (cmd == 'M' || cmd == 'm') {
    Axis* axis = parseInputAxis(p);
    double destination = parseNumber(p);
    axis->setDestination(destination);

  // Home (referencing) (currently only supports referencing all (not HX or HY)
  } else if (cmd == 'H' || cmd == 'h') {
    p.getWriter() << "Referencing...\n";
    for (int i = 0; p.axes[i] != NULL; i++) {
      p.axes[i]->startReferencing();
    }

  // Wait or sleep for some time
  } else if (cmd == 'w' || cmd == 'W') {
    double waitTime = parseNumber(p);
    p.sleepMs(waitTime);

  // Move forward
  } else if (cmd == '+') {
    Axis* axis = parseInputAxis(p);
    axis->rotate(FORWARD);

  // Move backward
  } else if (cmd == '-') {
    Axis* axis = parseInputAxis(p);
    axis->rotate(REVERSE);
  }

  p.getWriter() << "Cmd: " << cmd << "\n";

  for (int i = 0; p.axes[i] != NULL; i++) {
    p.axes[i]->afterInput();
  }
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
      p.getWriter() << "Error, received an action command when previous was not over.\n";
    } else {
      p.isWorking = true;
      parseActionCommand(cmd, p);
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

void oldMyLoop(Program& p) {
  if (p.isWorking) {

    if (p.inputAvailable()) {
      int incomingByte = p.getByte();
      if (incomingByte < 0) {
        return;
      }
      char cmd = (char) incomingByte;
      p.getWriter() << MESSAGE_RECEIVED << cmd << '\n';

      if (cmd == 's' || cmd == 'S') {
        for (int i = 0; p.axes[i] != NULL; i++) {
          p.axes[i]->stop();
        }
        p.isWorking = false;
      } else if (cmd == '?') {
        p.getWriter() << "\n" << MESSAGE_JSON << "\n";
        serialize<Writer>(p, p.getWriter());
        p.getWriter() << "\n";
      } else if (cmd == '@') { // asking for position
        p.getWriter() << MESSAGE_INVALID_PENDING << '\n';
      } else if (cmd == '\r' || cmd == '\n') { // ignore
      } else {
        p.getWriter() << MESSAGE_INVALID_PENDING << '\n';
      }
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
    
  } else if (!p.inputAvailable()) {
    p.sleepMs(10);
    return;
  } else {

    p.isWorking = true;
    
    for (int i = 0; p.axes[i] != NULL; i++) {
      p.axes[i]->prepare(p.getCurrentTime());
    }

    char input[256];
    if (p.getInput(input, 250)) {
      p.getWriter() << MESSAGE_RECEIVED << '\n';
    } else {
      p.getWriter() << MESSAGE_INVALID_INPUT << '\n';
      return;
    }
    
    int cursorPos = parseInput(input, p, 0);

    for (int i = 0; p.axes[i] != NULL; i++) {
      p.axes[i]->afterInput();
    }

  }
}

#endif
