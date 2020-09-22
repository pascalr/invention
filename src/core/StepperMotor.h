#ifndef STEPPER_MOTOR_H
#define STEPPER_MOTOR_H

// Pour le mouvement: voir thirdparty/mathematics-of-motion-control-profiles-article
// Plus tard déplacer en courbe S. Pour l'instant en courbe trapézoïdale.
// Le vitesse initiale et finale (pas zéro pour un stepper motor) est
// déterminé par la variable MAX_STEP_DELAY

#include "Motor.h"
#include "../lib/ArduinoMock.h"
#include "referencer.h"

#define MAX_STEP_DELAY 5000 // us
#define US_PER_S 1000000.0

class StepperMotor : public Motor {
  public:

    StepperMotor(Writer& writer, char theName) : Motor(writer, theName) {
      m_is_step_high = false;
      isMotorEnabled = true;
      forceRotation = false;
      m_position_steps = 0;

      m_default_max_speed = 1;
      m_acceleration = 0.2; // tour/s^2 [turn/sec^2]

    }

    void setAcceleration(double accel) {
      m_acceleration = accel;
    }

    void setDefaultMaxSpeed(double s) {
      m_default_max_speed = s;
    }

    void setMaxSpeed(double s) {
      m_max_speed = s;
    }

    double getAcceleration() {
      return m_acceleration;
    }

    double getAccelerationInUnits() {
      return m_acceleration * m_steps_per_turn / stepsPerUnit;
    }

    double getDefaultMaxSpeed() {
      return m_default_max_speed;
    }
    
    double getMaxSpeed() {
      return m_max_speed;
    }
    

    // Linear axes units are mm. Rotary axes units are degrees.
    // Number of steps per turn of the motor * microstepping / distance per turn
    // The value is multiplied by two because we have to write LOW then HIGH for one step
    void setStepsPerUnit(double ratio) {
      stepsPerUnit = ratio;
    }

    void setStepsPerTurn(double ratio) {
      m_steps_per_turn = ratio;
    }

    double getStepsPerTurn() {
      return m_steps_per_turn;
    }

    double getStepsPerUnit() {
      return stepsPerUnit;
    }

    void setupPins(int enabledPin, int dirPin, int stepPin) {
      m_enabled_pin = enabledPin;
      m_dir_pin = dirPin;
      m_step_pin = stepPin;

      pinMode(m_enabled_pin, OUTPUT);
      pinMode(m_dir_pin, OUTPUT);
      pinMode(m_step_pin, OUTPUT);
    }

    void rotate(bool direction) {
      setMotorDirection(direction);
      forceRotation = true;
      setMotorEnabled(true);
    }

    double getPosition() {
      return m_position_steps / stepsPerUnit;
    }

    void stop() {
      //setMotorsEnabled(false);
      setDestination(getPosition());
      forceRotation = false;
      is_referencing = false;
    }

    void setPosition(double pos) {
      m_position_steps = pos * stepsPerUnit;
    }

    void setPositionSteps(double posSteps) {
      m_position_steps = posSteps;
    }

    long getPositionSteps() {
      return m_position_steps;
    }

    double getDestinationSteps() {
      return getDestination() * stepsPerUnit;
    }

    double getDestinationTurns() {
      return getDestination() * m_steps_per_turn;
    }

    void updateDirection() {
      setMotorDirection(getDestination() > getPosition());
    }
    
    void setMotorEnabled(bool value) {
      digitalWrite(m_enabled_pin, LOW); // FIXME: ALWAYS ENABLED
      //digitalWrite(m_enabled_pin, value ? LOW : HIGH);
      isMotorEnabled = value;
    }

  //protected:
    // Linear axes units are mm. Rotary axes units are degrees.
    double stepsPerUnit;
    double m_steps_per_turn;
    
    int m_enabled_pin;
    int m_step_pin;
    int limitSwitchPin;
    
    bool isMotorEnabled;
    bool forceRotation;

    // TODO: All the units should simply be steps... NOOOOOOOOOO All units are in turns!
    // WARNING: THIS ONLY WORKS AT THE BEGINNING, DOES NOT COMPUTE ALL THE TIME
    unsigned long timeToReachDestinationUs() {
      return m_time_to_reach_middle_us * 2;
    }

    // tours [turns]
    double distanceToReachDestination() {
      double distance = (getDestination() - getPosition()) * stepsPerUnit / m_steps_per_turn; // tr
      distance *= (distance < 0) ? -1 : 1;
      return distance;
    }

    void calculateMovement() {

      double halfDistance = distanceToReachDestination() / 2;
      double v0 = US_PER_S / MAX_STEP_DELAY / m_steps_per_turn;

      m_max_speed_reached = sqrt(2 * m_acceleration * halfDistance + v0*v0); // tr/s
      
      if (m_max_speed_reached > m_max_speed) {m_max_speed_reached = m_max_speed;} // s

      double timeToAccelerate = (m_max_speed_reached - v0) / m_acceleration; // s
      m_time_to_reach_middle_us = ((unsigned long) (timeToAccelerate * US_PER_S)); // us
      double timeConstantSpeed = 0;

      if (m_max_speed_reached == m_max_speed) {

        double distanceAccelerating = v0*timeToAccelerate + 0.5 * m_acceleration * timeToAccelerate * timeToAccelerate; // tr
        double halfDistanceLeft = halfDistance - distanceAccelerating; // tr
        timeConstantSpeed = halfDistanceLeft / m_max_speed; // s
        m_time_to_reach_middle_us += ((unsigned long) (timeConstantSpeed * US_PER_S)); // us
      }
     
      m_time_to_start_decelerating_us = ((unsigned long)((timeConstantSpeed*2.0 + timeToAccelerate) * US_PER_S)); // us
      //std::cout << "m_time_to_start_decelerating_us: " << m_time_to_start_decelerating_us << std::endl;
      //std::cout << "m_time_to_reach_middle_us : " << m_time_to_reach_middle_us << std::endl;
      //std::cout << "timeToAccelerate : " << timeToAccelerate << std::endl;
    }

    int setDestination(double dest) {
      if (is_referenced == false) {return ERROR_AXIS_NOT_REFERENCED;}

      int status = Motor::setDestination(dest);
      if (status < 0) {return status;}

      calculateMovement();
      return 0;
    }

    double getCurrentSpeed() {
      return m_speed;
    }

    //virtual unsigned long calculateNextStepDelay(unsigned long timeSinceStart) {

    //  double distanceDecelerating = 0.5 * m_speed * m_speed / m_acceleration; // tr
    //  double position = getPosition();
    //  double distanceToGo = abs(getDestination() - position);
    //  double distanceToCollision = isForward ? getMaxPosition() - position : position - getMinPosition();

    //  // decelerate
    //  if (distanceDecelerating > distanceToGo || distanceDecelerating > distanceToCollision) {
    //   
    //    cout << "Decelerating!" << endl;
    //    m_speed -= m_acceleration * (m_next_step_time / 1000000.0); // tr/s

    //  // accelerate
    //  } else if (m_speed < getMaxSpeed()) {

    //    cout << "Accelerating!" << endl;
    //    m_speed += m_acceleration * (m_next_step_time / 1000000.0); // tr/s
    //  }
    //  
    //  unsigned long stepDelay = ((unsigned long)(1000000.0 / (m_speed * m_steps_per_turn))); // us

    //  return stepDelay > MAX_STEP_DELAY ? MAX_STEP_DELAY : stepDelay;
    //}

    //unsigned int microsteps = 16;

    unsigned long calculateNextStepDelay(unsigned long timeSinceStart) {

      if (forceRotation) {return MAX_STEP_DELAY;}

      // Time to accelerate
      // Accelerate or go top speed
      if (timeSinceStart <= m_time_to_reach_middle_us) {
        
        m_speed = m_acceleration * (timeSinceStart / US_PER_S); // tr/s
        if (m_speed > m_max_speed) {m_speed = m_max_speed;} // tr/s

      // Decelerate
      } else if (timeSinceStart > m_time_to_start_decelerating_us) {

        //std::cout << "Decelerating.\n";
        double t_s = (timeSinceStart - m_time_to_start_decelerating_us) / US_PER_S; // s
        m_speed = m_max_speed_reached - (m_acceleration * t_s); // tr/s
      }

      unsigned long stepDelay = ((unsigned long)(US_PER_S / (m_speed * m_steps_per_turn))); // us

      return stepDelay > MAX_STEP_DELAY ? MAX_STEP_DELAY : stepDelay;
    }

    unsigned long next_step_delay = 0;
    unsigned long lost_time = 0;

    void turnOneStep(unsigned long timeSinceStart) {
      digitalWrite(m_step_pin, m_is_step_high ? LOW : HIGH);
      m_is_step_high = !m_is_step_high;
      m_position_steps = m_position_steps + (isForward ? 1 : -1);

      lost_time = timeSinceStart - next_step_time;
      next_step_delay = calculateNextStepDelay(timeSinceStart); 
      next_step_time = timeSinceStart + next_step_delay - lost_time;
    }

    bool m_is_step_high;

    long m_position_steps;

    double m_max_speed_reached;
    unsigned long next_step_time; // us
    unsigned long m_start_time; // us
    unsigned long m_time_to_reach_middle_us; // us
    unsigned long m_time_to_start_decelerating_us; // us
   
    double m_acceleration; // tour/s^2 [turn/sec^2]
    double m_max_speed; // tour/s [turn/sec]
    double m_default_max_speed; // tour/s [turn/sec]
    double m_speed;

    LimitSwitchReferencer referencer;
    Referencer& getReferencer() {
      return referencer;
    }

    // TODO:
    // bool m_jog_forward; No more forceRotation.
    // bool m_jog_reverse; No more isForward.
    
    // Returns true if the axis is still working.
    virtual bool handleAxis(unsigned long currentTime) {
      //unsigned int delay = getDelay();

      if (handleReferencing(currentTime)) {return true;}
      
      if (forceRotation || !isDestinationReached()) {
        unsigned long timeSinceStart = timeDifference(m_start_time, currentTime); // us
        if (timeSinceStart >= next_step_time) {
          turnOneStep(timeSinceStart);
        }
        return true;
      } else {
        m_speed = 0;
      }
      return false;
    }

  protected:

    void doStartReferencing() {
    }

    // Resets some stuff.
    virtual void prepare(unsigned long time) {
      m_start_time = time;
      next_step_time = 0;
      m_speed = 0;
      m_max_speed = m_default_max_speed;
    }
   
    // Pretty sure this does not work... 
    virtual void run(unsigned long currentTime, double speedRPM) {
      next_step_time = ((unsigned long)(US_PER_S / (speedRPM / 60.0 * m_steps_per_turn))); // us
      unsigned long timeSinceStart = timeDifference(m_start_time, currentTime); // us
      if (currentTime >= next_step_time) {
        turnOneStep(timeSinceStart);
      }
    }

};

class MotorT : public StepperMotor {
  public:

    MotorT(Writer& writer, char theName) : StepperMotor(writer, theName) {
    }

    void doStartReferencing() {
      referenceReached();
    }

    void referenceReached() {
      Motor::referenceReached();

      //int total = 100;
      //double sum;
      //for (int i = 0; i < total; i++) {
      //  sum += analogRead(PIN_THETA_POT);
      //}
      //double mean = sum/total;
      //double degreesPerAnalog = 180.0 / (POT_180_VAL - POT_0_VAL);
      //double pos = degreesPerAnalog * mean;
      double pos = 90.0;
      
      setPosition(pos);
      setDestination(pos);
    }

};

#endif
