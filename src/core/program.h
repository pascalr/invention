#ifndef PROGRAM_H
#define PROGRAM_H

#include <ctype.h>
#include "axis.h"

class Program {
  public:
    Program(Writer& writer) : axisY(writer, 'Y'), axisX(writer, 'X'), axisA(writer, 'A'),
        axisB(writer, 'B'), axisZ(writer, 'Z', &axisX), axisT(writer, 'T') {
    }

    virtual Writer& getWriter() = 0;
    virtual unsigned long getCurrentTime() = 0;
    virtual void sleepMs(int time) = 0;
    virtual bool inputAvailable() = 0;
    virtual bool getInput(char* buf, int size) = 0;

    VerticalAxis axisY;
    HorizontalAxis axisX;
    Axis axisA;
    Axis axisB;
    ZAxis axisZ;
    Axis axisT;
      
    Axis* axes[10] = {&axisX, &axisY, &axisT, &axisA, &axisB, &axisZ, NULL}; // FIXME: CAREFULL WITH SIZE!!!
    //Axis* axes[] = {&axisX, &axisY, &axisT, &axisA, &axisB, &axisZ, NULL};
    
    bool isWorking = false;

    friend Writer& operator<<(Writer& writer, const char* theString);

    void serialize(Writer& out) {
      out << "{";
      for (int i = 0; axes[i] != NULL; i++) {
        out << "\"axis_" << axes[i]->name << "\": ";
        out << axes[i];
        if (axes[i+1] != NULL) {
          out << ", ";
        }
      }
      out << "}";
    }
};

Writer& operator<<(Writer& out, const Program &p) {
  out << "{";
  for (int i = 0; p.axes[i] != NULL; i++) {
    out << "\"axis_" << p.axes[i]->name << "\": ";
    p.axes[i]->serialize(out);
    if (p.axes[i+1] != NULL) {
      out << ", ";
    }
  }
  return out << "}";
}

bool isNumberSymbol(char c) {
  return (c >= '0' && c <= '9') || c == '.' || c == '-';
}

int parseMove(Axis** axes, const char* cmd, int oldCursor) {
  int i;
  for (i = oldCursor; cmd[i] != '\0'; i++) {

    /*if (cmd[i] == 'Z' || cmd[i] == 'z') {

      Axis* axisT = axisByLetter(axes, 'T');
      HorizontalAxis* axisX = (HorizontalAxis*)axisByLetter(axes, 'X');
            
      if (destination >= 0 && destination <= RAYON && axisT && axisX) {
        double angle = asin(destination / RAYON) * 180.0 / PI;
        axisT->setDestination(angle);
        axisT->setMotorEnabled(true);

        axisT->setFollowingAxis(axisX);
        axisX->setMotorEnabled(true);
        axisX->updateShouldGoForward();
        
        double deltaX = (RAYON * cos(angle * PI / 180)) - (RAYON * cos(axisT->getPosition() * PI / 180));
        axisX->setDestination(axisX->getPosition() + (deltaX * shouldGoForward));
        axisX->setMotorEnabled(true);
      }

      /*Axis* axisT = axisByLetter(axes, 'T');      
      
      if (destination > 0 && destination <= RAYON && axisT) {
        double angle = asin(destination / RAYON) * 180.0 / PI;
        axisT->setDestination(angle);
        axisT->setMotorEnabled(true);

        HorizontalAxis* axisX = (HorizontalAxis*)axisByLetter(axes, 'X');
        int shouldGoForward = axisX->getPosition() < axisX->maxPosition / 2 ? -1 : 1;
        double deltaX = (RAYON * cos(angle * PI / 180)) - (RAYON * cos(axisT->getPosition() * PI / 180));
        axisX->setDestination(axisX->getPosition() + (deltaX * shouldGoForward));
        axisX->setMotorEnabled(true);
      }
    } else {*/
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
    p.getWriter() << p;
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

void myLoop(Program& p) {
  if (p.isWorking) {

    if (p.inputAvailable()) {

      char c_str[12];
      if (p.getInput(c_str, 10)) {
        p.getWriter() << MESSAGE_RECEIVED << '\n';
      } else {
        p.getWriter() << MESSAGE_INVALID_INPUT << c_str << '\n';
        return;
      }

      if (strlen(c_str) != 1) {
        p.getWriter() << MESSAGE_INVALID_PENDING << c_str << '\n';
        return;
      }
      char cmd = c_str[0];
      if (cmd == 's' || cmd == 'S') {
        for (int i = 0; p.axes[i] != NULL; i++) {
          p.axes[i]->stop();
        }
        p.isWorking = false;
      } else if (cmd == '?') {
        p.getWriter() << "\n" << MESSAGE_JSON << "\n";
        p.getWriter() << p;
        p.getWriter() << "\n";
      } else if (cmd == '@') { // asking for position
        p.getWriter() << MESSAGE_INVALID_PENDING << '\n';
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
