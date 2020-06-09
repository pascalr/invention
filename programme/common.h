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

void parseMove(Axis** axes, const char* cmd) {
  for (int i = 0; cmd[i] != '\0'; i++) {

    double destination = atof(cmd+i+1);

    if (cmd[i] == 'Z' || cmd[i] == 'z') {

      Axis* axisT = axisByLetter(axes, 'T');      
      
      if (destination > 0 && destination <= RAYON && axisT) {
        double angle = asin(destination / RAYON) * 180.0 / PI;
        axisT->setDestination(angle);

        HorizontalAxis* axisX = (HorizontalAxis*)axisByLetter(axes, 'X');
        int shouldGoForward = axisX->getPosition() < axisX->maxPosition / 2 ? -1 : 1;
        double deltaX = (RAYON * cos(angle * PI / 180)) - (RAYON * cos(axisT->getPosition() * PI / 180));
        axisX->setDestination(axisX->getPosition() + (deltaX * shouldGoForward));
      }
    } else {
      Axis* axis = axisByLetter(axes, cmd[i]);
      if (axis) {
        axis->setDestination(destination);
      }
    }
  }
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

void parseInput(char* input, int size, Writer* writer, Axis** axes) {
  writer->doPrint("Cmd: ");
  writer->doPrintLn(input);
  if (input[0] == 'M' || input[0] == 'm') {
    parseMove(axes, input+1);
  } else if (input[0] == 's' || input[0] == 'S') { // stop
    //setMotorsEnabled(false);
    for (int i = 0; axes[i] != NULL; i++) {
      axes[i]->stop();
    }
  } else if (input[0] == 'H' || input[0] == 'h') { // home reference (eg. H, or HX, or HY, ...)
    writer->doPrintLn("Referencing...");
    if (size == 1) {
      for (int i = 0; axes[i] != NULL; i++) {
        axes[i]->startReferencing();
      }
    } else {
      Axis* axis = axisByLetter(axes, input[1]);
      if (axis) {
        axis->startReferencing();
      }
    }
  } else if (input == "?") { // debug info
    for (int i = 0; axes[i] != NULL; i++) {
      printDebugAxis(axes[i], writer);
    }
  } else if (input[0] == '+') {
    Axis* axis = axisByLetter(axes, input[1]);
    if (axis) {
      axis->rotate(CW);
    }
  } else if (input[0] == '-') {
    Axis* axis = axisByLetter(axes, input[1]);
    if (axis) {
      axis->rotate(CCW);
    }
  }
}

#endif
