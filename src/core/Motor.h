#ifndef MOTOR_H
#define MOTOR_H

#include "axis.h"
#include "../lib/ArduinoMock.h"

// TODO: The referencing method should be independant of the motor.
// This should be in another class.
// class ReferenceMethod
// class LimitSwitchReference
// class PotReference
// class VisualReference

class Motor : public Axis {
  public:
    Motor(Writer& writer, char name) : Axis(writer, name) {
    }

    virtual void rotate(bool direction) = 0;
    
    virtual void setPosition(double pos) = 0;

    void setMotorDirection(bool forward) {
      bool val = forward ? LOW : HIGH;
      val = m_reverse_motor_direction ? !val : val;
      digitalWrite(m_dir_pin, val);
      isForward = forward;
    }
    
    void setReverseMotorDirection(bool val) {
      m_reverse_motor_direction = val;
    }

    virtual void referenceReached() {
      stop();
      isReferenced = true;
      isReferencing = false;
      setPosition(0);
      setDestination(0);
      //setMotorEnabled(true);
      m_writer << "Done referencing axis " << getName() << '\n';
    }

    virtual void stop() = 0;

    virtual void startReferencing() {
      referenceReached(); // FIMXE: Temporary
      /*isReferencing = true;
      setMotorDirection(CCW);
      setMotorEnabled(true);*/
    }
    
    virtual bool handleAxis(unsigned long currentTime) = 0;
    
    virtual void prepare(unsigned long time) = 0;

    bool isReferenced = false;
    bool isReferencing = false;
    bool isForward = false;

  protected:
    
    int m_dir_pin;
    bool m_reverse_motor_direction = false;
    
};

#endif
