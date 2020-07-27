#ifndef DC_MOTOR_H
#define DC_MOTOR_H

#include "Motor.h"

#include "writer.h"
#include "../config/constants.h"
#include "../utils/utils.h"
#include <math.h>
#include "../lib/ArduinoMock.h"

#ifndef ARDUINO
#include <iostream>
using namespace std;
#endif

class DCMotor : public Motor {
  public:

    DCMotor(Writer& writer, char name) : Motor(writer, name) {
      m_speed = 0;
    }

    void setSpeed(uint8_t speed) {
      analogWrite(m_pwm_pin, speed);
    }
        
    void rotate(bool direction) {
      setMotorDirection(direction);
      setSpeed(10);
    }

    void setupPins(uint8_t dirPin, uint8_t pwmPin) {
      m_pwm_pin = pwmPin;
      m_dir_pin = dirPin;

      pinMode(m_dir_pin, OUTPUT);
      pinMode(m_pwm_pin, OUTPUT);
    }

    void setPosition(double pos) {
    }
    double getPosition() {
      return 0;
    }

    void stop() {
      setSpeed(0);
    }

    bool handleAxis(unsigned long currentTime) {
      if (m_speed == 0) {return false;}

      // TODO: Handle acceleration and deceleration

      return true;
    }

    void prepare(unsigned long time) {
    }

  protected:

    uint8_t m_dir_pin;
    uint8_t m_pwm_pin;
    int m_speed = 10; // Duty cycle from 0 to 255
};

#endif
