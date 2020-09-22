#ifndef MOTOR_H
#define MOTOR_H

#include "writer/writer.h"
#include "../utils/constants.h"
#include "../utils/utils.h"
#include <math.h>
#include "../lib/ArduinoMock.h"
#include "referencer.h"

#ifndef ARDUINO
#include <iostream>
using namespace std;
#endif

#define MAX_STEP_DELAY 10000 // us (5 ms)

// TODO: The referencing method should be independant of the motor.
// This should be in another class.
// class ReferenceMethod
// class LimitSwitchReference
// class PotReference
// class VisualReference

class Motor {
  public:
    Motor(Writer& writer, char name) : m_writer(writer) {
      m_name = name;
      m_destination = -1;
      m_max_position = 999999;
      m_min_position = 0;
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
      is_referenced = true;
      is_referencing = false;
      setPosition(0);
      setDestination(0);
      //setMotorEnabled(true);
      m_writer << "Done referencing axis " << getName() << '\n';
    }

    virtual void stop() = 0;

    virtual Referencer& getReferencer() = 0;

    virtual void run(unsigned long currentTime, double speedRPM) = 0;

    bool handleReferencing(unsigned long currentTime) {

      if (!is_referencing) {return false;}

      if (getReferencer().isReferenceReached()) {
        referenceReached();
        return false;
      }

      run(currentTime, REFERENCE_SPEED_RPM);
      return true;
    }

    void startReferencing() {
      isWorking = true;
      is_referenced = false;
      is_referencing = true;
      setMotorDirection(REVERSE);
      doStartReferencing();
    }
    
    bool isDestinationReached() {
      if (!is_referenced) {return true;} // ???
      return (isForward && getPosition() >= getDestination()) ||
             (!isForward && getPosition() <= getDestination());
    }

    void prepareWorking(unsigned long time) {
      isWorking = true;
      prepare(time);
    }

    bool is_referenced = false;
    bool is_referencing = false;
    bool isForward = false;
    bool isWorking = false;

    char getName() {
      return m_name;
    }
    
    double getDestination() {
      return m_destination;
    }

    virtual double getPosition() = 0;

    virtual int setDestination(double dest) {

      if (dest >= m_max_position + 0.1) {return ERROR_DESTINATION_TOO_HIGH;}
      if (dest <= m_min_position - 0.1) {return ERROR_DESTINATION_TOO_LOW;}

      m_destination = dest;
      updateDirection();

      return 0;
    }
    
    virtual void updateDirection() {}

    void setMaxPosition(double maxP) {
      m_max_position = maxP;
    }

    double getMaxPosition() {
      return m_max_position;
    }
    
    double getMinPosition() {
      return m_min_position;
    }

    void setMinPosition(double pos) {
      m_min_position = pos;
    }
    
    virtual bool handleAxis(unsigned long currentTime) = 0;

  protected:
    Writer& m_writer;
    char m_name;
    double m_destination;
    double m_max_position;
    double m_min_position;

    virtual void doStartReferencing() = 0;
    
    virtual void prepare(unsigned long time) = 0;

    int m_dir_pin;
    bool m_reverse_motor_direction = false;
    
};

#include "program.h"

#endif
