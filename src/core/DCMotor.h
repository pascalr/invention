#ifndef DC_MOTOR_H
#define DC_MOTOR_H

#include "Motor.h"

#include "writer.h"
#include "../config/constants.h"
#include "../utils/utils.h"
#include <math.h>
#include "../lib/ArduinoMock.h"
#include "Encoder.h"

#ifndef ARDUINO
#include <iostream>
using namespace std;
#endif

class DCMotor : public Motor {
  public:

    DCMotor(Writer& writer, char name) : Motor(writer, name) {
      m_duty_cycle = 0;
    }

    void setDutyCycle(uint8_t speed) {
      m_duty_cycle = speed;
      analogWrite(m_pwm_pin, speed);
    }

    void grab(int dutyCycle) {
      setMotorDirection(REVERSE);
      setDutyCycle(dutyCycle);
      setDestination(-9999999999);
    }

    void release() {
      setMotorDirection(REVERSE);
      setDestination(90 * 8 * 0.75);
    }
        
    void rotate(bool direction) {
      setMotorDirection(direction);
      setDutyCycle(25);
    }

    void setupPins(uint8_t dirPin, uint8_t pwmPin, uint8_t stepPin) {
      m_pwm_pin = pwmPin;
      m_dir_pin = dirPin;
  
      m_encoder.setStepPin(stepPin);

      pinMode(m_dir_pin, OUTPUT);
      pinMode(m_pwm_pin, OUTPUT);
    }

    void setPosition(double pos) {
      m_encoder.setPosition(pos);
    }

    double getPosition() {
      return m_encoder.getPosition();
    }

    void stop() {
      setDutyCycle(0);
    }



  protected:

    virtual void doStartReferencing() {
      isReferencing = true;
      isReferenced = false;
      setMotorDirection(REVERSE);
      setDutyCycle(25);
    }

    void prepare(unsigned long time) {
      m_encoder.prepare(time);
    }

    bool handleAxis(unsigned long currentTime) {
      if (m_duty_cycle == 0) {return false;}

      m_encoder.checkPosition(currentTime, isForward);

      if (isDestinationReached()) {
        setDutyCycle(0);
      }

      if (m_encoder.isRpmCalculated() && m_encoder.getRpm() < 0.01) {
        if (isReferencing) {
          referenceReached();
        }
	      return false;
      }
        
      // TODO: Handle acceleration and deceleration
      return true;
    }

    Encoder m_encoder;
    uint8_t m_pwm_pin;
    int m_duty_cycle = 10; // Duty cycle from 0 to 255
};

#endif
