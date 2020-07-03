#ifndef COMMON_H
#define COMMON_H

#include <ctype.h>

#ifndef PI
#define PI 3.1415926535897932384626433832795
#endif

#define MESSAGE_RECEIVED "ok"
#define MESSAGE_DONE "done"
#define MESSAGE_INVALID "invalid"

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

int parseInput(const char* input, Writer* writer, Axis** axes, int oldCursor) {

  // Should the cursor passed to the function be a pointer?
  int cursor = oldCursor + 1;
  int size = strlen(input);
  char cmd = input[oldCursor];
  
  if (cmd == 'M' || cmd == 'm') {
    cursor = parseMove(axes, input, cursor);
  } else if (cmd == 's' || cmd == 'S') { // stop
    for (int i = 0; axes[i] != NULL; i++) {
      axes[i]->stop();
    }
    cursor = size; // Disregard everything else after the stop command.
  } else if (cmd == 'H' || cmd == 'h') { // home reference (eg. H, or HX, or HY, ...)
    writer->doPrint("Referencing...\n");
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
    #ifndef DEBUG
      delay(waitTime);
    #endif
  } else if (cmd == '?') {
    for (int i = 0; axes[i] != NULL; i++) {
      axes[i]->serialize();
    }
  } else if (cmd == '+') {
    Axis* axis = axisByLetter(axes, input[cursor]);
    cursor++;
    if (axis) {
      axis->rotate(FORWARD);
    }
    // TODO: Handle error
  } else if (cmd == '-') {
    Axis* axis = axisByLetter(axes, input[cursor]);
    cursor++;
    if (axis) {
      axis->rotate(REVERSE);
    }
    // TODO: Handle error
  }

  writer->doPrint("Cmd: ");
  writer->doPrint(input+oldCursor);
  writer->doPrint("\n");
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

#endif
