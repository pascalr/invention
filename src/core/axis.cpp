#include "axis.h"

#include <ctype.h> 

Axis::Axis(Writer& writer, char theName) : m_writer(writer) {
  name = theName;
  destination = -1;
  previousStepTime = 0;
  isStepHigh = false;
  isMotorEnabled = false;
  isForward = false;
  isReferenced = false;
  isReferencing = false;
  speed = 500;
  forceRotation = false;
  maxPosition = 999999;
  m_position_steps = 0;
  //m_destination_steps = 0;
  m_reverse_motor_direction = false;
  m_min_position = 0;
}

Axis* axisByLetter(Axis** axes, char letter) {
  for (int i = 0; axes[i] != 0; i++) {
    if (toupper(letter) == axes[i]->name) {
      return axes[i];
    }
  }

  return 0;
}

bool Axis::handleAxis(unsigned long currentTime) {
  unsigned int delay = getDelay();
  
  if (isReferencing) {
    return moveToReference();
  } else if (canMove() && (forceRotation || !isDestinationReached())) {
    unsigned long deltaTime = currentTime - previousStepTime;
    if (deltaTime > delay) {
      turnOneStep();
      // TODO: Instead of this, call a function named prepareToMove, that updates the previousStepTime
      if (deltaTime > 2*delay) {
        previousStepTime = currentTime; // refreshing previousStepTime when it is the first step and the motor was at a stop
      } else {
        previousStepTime = previousStepTime + delay; // This is more accurate to ensure all the motors are synchronysed
      }
    }
    return true;
  }
  return false;
}
