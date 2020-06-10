#ifndef COMMON_H
#define COMMON_H

#include <ctype.h>

#ifndef PI
#define PI 3.1415926535897932384626433832795
#endif

Axis* axisByLetter(Axis** axes, char letter) {
  for (int i = 0; axes[i] != NULL; i++) {
    if (toupper(letter) == axes[i]->name) {
      return axes[i];
    }
  }

  return NULL;
}

bool isNumberSymbol(char c) {
  return (c >= '0' && c <= '9') || c == '.' || c == '-';
}

int parseMove(Axis** axes, const char* cmd, int oldCursor) {
  int i;
  for (i = oldCursor; cmd[i] != '\0'; i++) {

    double destination = atof(cmd+i+1);

    if (cmd[i] == 'Z' || cmd[i] == 'z') {

      Axis* axisT = axisByLetter(axes, 'T');      
      
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
    } else {
      Axis* axis = axisByLetter(axes, cmd[i]);
      if (axis) {
        axis->setDestination(destination);
        axis->setMotorEnabled(true);
      } else if(!isNumberSymbol(cmd[i])) {
        return i; // End of move command, stop
      }
    }
  }
  return i;
}

void printDebugAxis(Axis* axis, Writer* writer) {
  // TODO: Turn the axis into a char[], then print the char[], so I can debug too.
  writer->doPrint("-Pos ");
  writer->doPrint(axis->name);
  writer->doPrint(": ");
  writer->doPrintLn(axis->getPosition());
  
  writer->doPrint("-Dest ");
  writer->doPrint(axis->name);
  writer->doPrint(": ");
  writer->doPrintLn(axis->destination);
  
  writer->doPrint("-Speed ");
  writer->doPrint(axis->name);
  writer->doPrint(": ");
  writer->doPrintLn(axis->speed);

  writer->doPrint("-CW ");
  writer->doPrint(axis->name);
  writer->doPrint(": ");
  writer->doPrintLn(axis->isClockwise);
  
  writer->doPrint("-Referenced ");
  writer->doPrint(axis->name);
  writer->doPrint(": ");
  writer->doPrintLn(axis->isReferenced);
  
  writer->doPrint("-Referencing ");
  writer->doPrint(axis->name);
  writer->doPrint(": ");
  writer->doPrintLn(axis->isReferencing);
  
  writer->doPrint("-Enabled ");
  writer->doPrint(axis->name);
  writer->doPrint(": ");
  writer->doPrintLn(axis->isMotorEnabled);
  
  writer->doPrint("-Step ");
  writer->doPrint(axis->name);
  writer->doPrint(": ");
  writer->doPrintLn(axis->isStepHigh);

  writer->doPrint("-Force ");
  writer->doPrint(axis->name);
  writer->doPrint(": ");
  writer->doPrintLn(axis->forceRotation);

  writer->doPrint("-PIN enabled ");
  writer->doPrint(axis->name);
  writer->doPrint(": ");
  writer->doPrintLn(writer->doDigitalRead(axis->enabledPin));
  
  writer->doPrint("-PIN dir ");
  writer->doPrint(axis->name);
  writer->doPrint(": ");
  writer->doPrintLn(writer->doDigitalRead(axis->dirPin));
  
  writer->doPrint("-PIN step ");
  writer->doPrint(axis->name);
  writer->doPrint(": ");
  writer->doPrintLn(writer->doDigitalRead(axis->stepPin));
  
  writer->doPrint("-PIN limit switch ");
  writer->doPrint(axis->name);
  writer->doPrint(": ");
  writer->doPrintLn(writer->doDigitalRead(axis->limitSwitchPin));

  writer->doPrint("-stepsPerUnit ");
  writer->doPrint(axis->name);
  writer->doPrint(": ");
  writer->doPrintLn(axis->stepsPerUnit);
}

int parseInput(const char* input, Writer* writer, Axis** axes, int oldCursor) {

  // Should the cursor passed to the function be a pointer?
  int cursor = oldCursor + 1;
  int size = strlen(input);
  char cmd = input[oldCursor];

  char* scmd = &cmd;
  writer->doPrint("Cmd: ");
  writer->doPrintLn(&cmd);
  /*char sint[5];
  itoa(cmd, sint, 10);
  writer->doPrintLn(sint);*/
  
  if (cmd == 'M' || cmd == 'm') {
    cursor = parseMove(axes, input, cursor);
  } else if (cmd == 's' || cmd == 'S') { // stop
    for (int i = 0; axes[i] != NULL; i++) {
      axes[i]->stop();
    }
    cursor = size; // Disregard everything else after the stop command.
  } else if (cmd == 'H' || cmd == 'h') { // home reference (eg. H, or HX, or HY, ...)
    writer->doPrintLn("Referencing...");
    if (size == 1) {
      for (int i = 0; axes[i] != NULL; i++) {
        axes[i]->startReferencing();
      }
    } else {
      Axis* axis = axisByLetter(axes, input[cursor]);
      cursor++;
      if (axis) {
        axis->startReferencing();
      }
      // TODO: Handle error
    }
  } else if (cmd == 'w' || cmd == 'W') { // wait or sleep for some time
    double waitTime = atof(input + cursor);
    while (isNumberSymbol(input[cursor])) {cursor++;}
    delay(waitTime);
  } else if (cmd == '?') {
    for (int i = 0; axes[i] != NULL; i++) {
      printDebugAxis(axes[i], writer);
    }
  } else if (cmd == '+') {
    Axis* axis = axisByLetter(axes, input[cursor]);
    cursor++;
    if (axis) {
      axis->rotate(CW);
    }
    // TODO: Handle error
  } else if (cmd == '-') {
    Axis* axis = axisByLetter(axes, input[cursor]);
    cursor++;
    if (axis) {
      axis->rotate(CCW);
    }
    // TODO: Handle error
  }

  return cursor;
}

#endif
