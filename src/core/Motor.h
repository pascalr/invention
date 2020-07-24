#ifndef MOTOR_H
#define MOTOR_H

#include "axis.h"

class Motor : public Axis {
  public:
    Motor(Writer& writer, char name) : Axis(writer, name) {
    }

    virtual void rotate(bool direction) = 0;
    
    virtual void referenceReached() = 0;
};

#endif
