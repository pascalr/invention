#ifndef _ENCODER_H
#define _ENCODER_H

#include "../utils/utils.h"
#include "../lib/ArduinoMock.h"

class Encoder {
  public:

    void checkPosition(unsigned long currentTime, bool isForward) {

      if (timeDifference(m_last_rpm_time, currentTime) > 100000) { // Calculate RPM every 100ms.
        // There are 8 steps per turn FIXME: Pass this as an argument to the class.
	      m_is_rpm_calculated = true;
        m_rpm = m_rpm_count / 8.0 * 60;
        m_last_rpm_time = currentTime;
        m_rpm_count = 0;
      }

      // Read about interrupt, maybe they are more efficient way to do this. I read somewhere.

      if (analogRead(m_step_pin) > 500) { // Should be digital maybe, useless to be analog
        m_step_was_high = true;
	digitalWrite(LED_BUILTIN, HIGH);
      } else if (m_step_was_high) {
        m_step_was_high = false;
        m_rpm_count += 1;
        m_position_steps += (isForward) ? 1 : -1;
      }
      //delay(500);
    }

    void setStepPin(uint8_t stepPin) {
      m_step_pin = stepPin;
    }

    void setPosition(long pos) {
      m_position_steps = pos;
    }

    double getPosition() {
      return m_position_steps;
    }

    void prepare(unsigned long time) {
      m_is_rpm_calculated = false;
      m_last_rpm_time = time;
      m_rpm_count = 0;
      m_step_was_high = analogRead(m_step_pin) > 500;
    }

    bool isRpmCalculated() {
      return m_is_rpm_calculated;
    }
   
    double getRpm() {
      return m_rpm;
    }
      
  protected:

    bool m_is_rpm_calculated;

    bool m_step_was_high;

    unsigned long m_last_rpm_time;
    long m_position_steps;

    double m_rpm;
    long m_rpm_count;
    
    uint8_t m_step_pin;
};

#endif
