#include "axis.h"

#include <ctype.h> 

Axis* axisByLetter(Axis** axes, char letter) {
  for (int i = 0; axes[i] != 0; i++) {
    if (toupper(letter) == axes[i]->getName()) {
      return axes[i];
    }
  }

  return 0;
}

/*
// WARNING THERE ARE NO CHECKS THAT THE AXIS SLOWS DOWN. IT SETS THE MAX SPEED SO IT WORKS.
void slowDownAxis(MotorAxis& axis, unsigned long time_us) {

  double dist = axis.distanceToReachDestination(); // tr
  double acceleration = axis.getAcceleration(); // tr/s^2
  double time_s = time_us / 1000000.0; // s

  // I used the equations:
  // x = 2*(0.5*a*t1^2) + v*t2
  // v = a*t1
  // t1 + t2 = t
  // where t1 = time accelerating, t2 = max speed duration
  // v = max speed, t = time to travel, x = distance to travel
  // And I got:
  // t1 = x/(a*t)
  // If this works I am so proud hahaha
  double t1 = dist/(acceleration*time_s); // s
  double maxSpeed = acceleration * t1; // tr/s
  axis.setMaxSpeed(maxSpeed);
  axis.calculateMovement();
}
*/
