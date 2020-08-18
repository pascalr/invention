#ifndef _REFERENCER_H
#define _REFERENCER_H

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


// Close gripper as much as possible and set default value.
class LimitSwitchReferencer {
  public:
    LimitSwitchReferencer() {}
    LimitSwitchReferencer(int pin) : m_pin(pin) {}
    bool isReferenceReached() {
      if (m_pin == -1) {return true;}
      digitalRead(m_pin);
    }
  protected:
    int m_pin = -1;
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
