#ifndef COMMON_H
#define COMMON_H

#include <ctype.h>

Axis* axisByLetter(Axis** axes, char letter) {
  for (int i = 0; i < NB_AXES; i++) {
    if (toupper(letter) == axes[i]->name) {
      return axes[i];
    }
  }

  return NULL;
}

// TODO parse float numbers too
int numberLength(String str) {
  int i;
  for (i = 0; i < str.length(); i++) {
    if (str[i] < '0' || str[i] > '9') {break;}
  }
  return i;
}

void parseMove(Axis** axes, String cmd) {
  for (int i = 0; i < cmd.length(); i++) {
    int nbLength = numberLength(cmd.substring(i+1));
    if (cmd[i] == 'Z' || cmd[i] == 'z') {
      int destination = cmd.substring(i+1,i+1+nbLength).toInt();
      Axis* axisT = axisByLetter(axes, 'T');
      if (destination > 0 && destination <= RAYON && axisT) {
        double angle = asin(destination / RAYON) * 180.0 / PI;
        axisT->setDestinationUnit(angle);
      }
    } else {
      Axis* axis = axisByLetter(axes, cmd[i]);
      if (axis) {
        axis->setDestinationUnit(cmd.substring(i+1,i+1+nbLength).toInt());
      }
    }
    i = i+nbLength;
  }
}

#endif
