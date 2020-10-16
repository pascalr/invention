#ifndef DC_MOTOR_H
#define DC_MOTOR_H

#include "Motor.h"

#include "writer/writer.h"
#include "../utils/constants.h"
#include "../utils/utils.h"
#include <math.h>
#include "../lib/ArduinoMock.h"
#include "Encoder.h"

class DCMotor : public Motor {
  public:

    DCMotor(Writer& writer, char name, int wheelNbHoles, double unitsPerTurn) : Motor(writer, name), encoder(wheelNbHoles, unitsPerTurn), referencer(encoder) {
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
      setMotorDirection(FORWARD);
      setDutyCycle(40);
      setDestination(90 * 8 * 0.75);
    }
        
    int getto(double destination) {
      setDestination(90 * 8 * 0.75);
      setDutyCycle(50);
      return 0;
    }

    void rotate(bool direction) {
      setMotorDirection(direction);
      setDutyCycle(50);
    }
   
    // already running, nothing to do here
    void run(unsigned long currentTime, double speedRPM) {}

    void setupPins(uint8_t dirPin, uint8_t pwmPin, uint8_t stepPin) {
      m_pwm_pin = pwmPin;
      m_dir_pin = dirPin;
  
      encoder.setStepPin(stepPin);

      pinMode(m_dir_pin, OUTPUT);
      pinMode(m_pwm_pin, OUTPUT);
    }

    void setPosition(double pos) {
      encoder.setPosition(pos);
    }

    double getPosition() {
      return encoder.getPosition();
    }

    void stop() {
      setDutyCycle(0);
      is_referencing = false;
    }

    Referencer& getReferencer() {
      return referencer;
    }
    
    bool handleAxis(unsigned long currentTime) {

#ifndef ARDUINO
      return false;
#endif

      if (m_duty_cycle == 0) {return false;}
      
      encoder.checkPosition(currentTime, isForward);

      if (handleReferencing(currentTime)) {return true;}

      if (isDestinationReached()) {
        setDutyCycle(0);
      }
      
      return !(encoder.isRpmCalculated() && encoder.getRpm() < 0.01);
    }
    
    Encoder encoder;

  protected:

    EncoderReferencer referencer;

    virtual void doStartReferencing() {
      setDutyCycle(50);
    }
    
    void prepare(unsigned long time) {
      encoder.prepare(time);
    }

    uint8_t m_pwm_pin;
    int m_duty_cycle = 10; // Duty cycle from 0 to 255
};

#endif
