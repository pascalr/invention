#ifndef COMMON_H
#define COMMON_H

#include <ctype.h>

#ifndef PI
#define PI 3.1415926535897932384626433832795
#endif

Axis* axisByLetter(Axis** axes, char letter) {
  for (int i = 0; i < NB_AXES; i++) {
    if (toupper(letter) == axes[i]->name) {
      return axes[i];
    }
  }

  return NULL;
}

// TODO parse float numbers too
// Ignores the letters at the beginning
int numberLength(const char* str0) {
  int i;
  bool started = false;
  for (i = 0; str0[i] != '\0'; i++) {
    if (str0[i] < '0' || str0[i] > '9' && started) {break;}
    else {started = true;}
  }
  return i;
}

double nextNumber(const char* str0) {
  int nbLength = numberLength(str0);
  char numberLetters[nbLength+1] = {0};
  memcpy(numberLetters, &str0[1], nbLength*sizeof(char));
  int number = atoi(numberLetters);
}

void parseMove(Axis** axes, const char* cmd) {
  for (int i = 0; cmd[i] != '\0'; i++) {
    
    double destination = nextNumber(cmd+i+1);
    
    if (cmd[i] == 'Z' || cmd[i] == 'z') {
      Axis* axisT = axisByLetter(axes, 'T');
      if (destination > 0 && destination <= RAYON && axisT) {
        double angle = asin(destination / RAYON) * 180.0 / PI;
        axisT->setDestinationUnit(angle);
      }
    } else {
      Axis* axis = axisByLetter(axes, cmd[i]);
      if (axis) {
        axis->setDestinationUnit(destination);
      }
    }
  }
}

#endif
