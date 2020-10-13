#ifndef _REFERENCER_H
#define _REFERENCER_H

#include "Encoder.h"

// There are many ways to do a reference:
// With a potentiometer
// With a limit switch
// With an encoder

/*
class ReferencingMotor : public Motor {
  public:
    ReferencingMotor(Motor& motor) : m_motor(motor) {
    }
    bool isReferenced() {
      return m_is_referenced;
    }
    bool isReferencing() {
      return m_is_referencing;
    }
    void start() {
    }
    void finish() {
      stop();
      m_is_referenced = true;
      m_is_referencing = false;
      m_motor.setPosition(0);
      m_motor.setDestination(0);
      //setMotorEnabled(true);
      m_motor.getWriterm_writer << "Done referencing axis " << getName() << '\n';
    }
  protected:
    bool m_is_referenced = false;
    bool m_is_referencing = false;
    Motor& m_motor;

};
*/

// Close gripper as much as possible and set default value.
//class GripperReferencer : public Referencer {
//};
//

class Referencer {
  public:
    virtual bool isReferenceReached() = 0;
};

// Close gripper as much as possible and set default value.
class LimitSwitchReferencer : public Referencer {
  public:
    LimitSwitchReferencer() {}
    LimitSwitchReferencer(int pin) : m_pin(pin) {
     pinMode(pin, INPUT_PULLUP);
    }
    bool isReferenceReached() {
#ifndef ARDUINO
      return true;
#endif
      if (m_pin == -1) {return true;}
      return digitalRead(m_pin) == LOW;
    }
  protected:
    int m_pin = -1;
};

// Moves until it cannot move anymore
class EncoderReferencer : public Referencer {
  public:
    EncoderReferencer(Encoder& encoder) : m_encoder(encoder) {}
    bool isReferenceReached() {
      return m_encoder.isRpmCalculated() && m_encoder.getRpm() < 0.01;
    }
  protected:
    Encoder& m_encoder;
};

// class PotentiometerReferencer analogRead(pot)

// Sets a given value
//class DefaultReferencer : public Referencer {
//  public:
//    void start() {
//      finish();
//    }
//};

#endif
