#ifndef MOTOR_H
#define MOTOR_H

#include "axis.h"
#include "../lib/ArduinoMock.h"
#include "program.h"

class Program;

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

    void startReferencing() {
      isWorking = true;
      doStartReferencing();
    }
    
    bool work(Program& p, unsigned long currentTime) {
      bool isCurrentlyWorking = handleAxis(currentTime);
      if (isWorking && !isCurrentlyWorking && doneWorkingCallback) {
        doneWorkingCallback(p);
        doneWorkingCallback = 0;
      }
      isWorking = isCurrentlyWorking;
      return isWorking;
    }

    bool isDestinationReached() {
      return (isForward && getPosition() >= getDestination()) ||
             (!isForward && getPosition() <= getDestination());
    }

    void prepareWorking(unsigned long time) {
      isWorking = true;
      prepare(time);
    }

    bool isReferenced = false;
    bool isReferencing = false;
    bool isForward = false;
    bool isWorking = false;

    void (*doneWorkingCallback)(Program& p) = 0;

  protected:

    virtual void doStartReferencing() = 0;
    
    virtual void prepare(unsigned long time) = 0;
    
    virtual bool handleAxis(unsigned long currentTime) = 0;

    int m_dir_pin;
    bool m_reverse_motor_direction = false;
    
};

#endif
