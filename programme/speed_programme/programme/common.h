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

void parseMove(Axis** axes, const char* cmd) {
  for (int i = 0; cmd[i] != '\0'; i++) {

    double destination = atof(cmd+i+1);
    
    if (cmd[i] == 'Z' || cmd[i] == 'z') {

      Axis* axisT = axisByLetter(axes, 'T');      
      
      if (destination > 0 && destination <= RAYON && axisT) {
        double angle = asin(destination / RAYON) * 180.0 / PI;
        axisT->setDestination(angle);

        HorizontalAxis* axisX = (HorizontalAxis*)axisByLetter(axes, 'X');
        int shouldGoForward = axisX->getPosition() < axisX->maxPosition / 2 ? 1 : -1;
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

#endif
