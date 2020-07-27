#ifndef STEPPER_MOTOR_H
#define STEPPER_MOTOR_H

#include "Motor.h"
#include "../lib/ArduinoMock.h"

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

    bool notGoingOutOfBounds() {
      double p = getPosition();
      return isForward ? p <= getMaxPosition() : p >= m_min_position;
    }

    bool canMove() {
      return isReferenced && isMotorEnabled && notGoingOutOfBounds();
    }

    double getPosition() {
      return m_position_steps / stepsPerUnit;
    }

    void stop() {
      //setMotorsEnabled(false);
      setDestination(getPosition());
      forceRotation = false;
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
      m_writer.doDigitalWrite(m_enabled_pin, LOW); // FIXME: ALWAYS ENABLED
      //digitalWrite(m_enabled_pin, value ? LOW : HIGH);
      isMotorEnabled = value;
    }


    virtual bool isDestinationReached() {
      //double destSteps = getDestinationSteps();
      //double posSteps = getPositionSteps();
      //return (isForward && posSteps >= destSteps) ||
      //       (!isForward && posSteps <= destSteps);
      return (isForward && getPosition() >= getDestination()) ||
             (!isForward && getPosition() <= getDestination());
    }

    virtual void referenceReached() {
      m_writer << "Done referencing axis " << getName() << '\n';
      isReferenced = true;
      isReferencing = false;
      setPositionSteps(0);
      setDestination(0);
      setMotorEnabled(true);
    }

    // Only the vertical axis moves in order to do a reference
    virtual bool moveToReference() {
      referenceReached();
      return true;
    }

    void setIsReferenced(bool isRef) {
      isReferenced = isRef;
    }

    void setIsReferencing(bool isRef) {
      isReferencing = isRef;
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

    // Get the delay untill the next step
    /*virtual double getDelay() {
      // TODO: The speed is not fixed.
      return speed;
    }*/

    /*double getSpeed() {
      double frequency = 1 / getDelay() * 1000000;
      double theSpeed = frequency / stepsPerUnit;
      return theSpeed;
    }*/

    // distance: steps, time: microseconds

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

      m_max_speed_reached = sqrt(2 * m_acceleration * halfDistance); // tr/s
      
      if (m_max_speed_reached > m_max_speed) {m_max_speed_reached = m_max_speed;} // s

      double timeToAccelerate = m_max_speed_reached / m_acceleration; // s
      m_time_to_reach_middle_us = ((unsigned long) (timeToAccelerate * 1000000.0)); // us

      if (m_max_speed_reached == m_max_speed) {

        double distanceAccelerating = 0.5 * m_acceleration * timeToAccelerate * timeToAccelerate; // tr
        double halfDistanceLeft = halfDistance - distanceAccelerating; // tr

        m_time_to_reach_middle_us += ((unsigned long) ((halfDistanceLeft / m_max_speed) * 1000000.0)); // us
      }
     
      m_time_to_start_decelerating_us = (m_time_to_reach_middle_us * 2) - ((unsigned long)(timeToAccelerate * 1000000.0)); // us
      //std::cout << "m_time_to_start_decelerating_us: " << m_time_to_start_decelerating_us << std::endl;
      //std::cout << "m_time_to_reach_middle_us : " << m_time_to_reach_middle_us << std::endl;
      //std::cout << "timeToAccelerate : " << timeToAccelerate << std::endl;
    }

    int setDestination(double dest) {
      if (isReferenced == false) {return ERROR_AXIS_NOT_REFERENCED;}

      int status = Axis::setDestination(dest);
      if (status < 0) {return status;}

      calculateMovement();
      return 0;
    }

    double getCurrentSpeed() {
      return m_speed;
    }

    /*virtual unsigned long calculateNextStepDelay(unsigned long timeSinceStart) {

      double distanceDecelerating = 0.5 * m_speed * m_speed / m_acceleration; // tr
      double position = getPosition();
      double distanceToGo = abs(getDestination() - position);
      double distanceToCollision = isForward ? getMaxPosition() - position : position - getMinPosition();

      // decelerate
      if (distanceDecelerating > distanceToGo || distanceDecelerating > distanceToCollision) {
       
        cout << "Decelerating!" << endl;
        m_speed -= m_acceleration * (m_next_step_time / 1000000.0); // tr/s

      // accelerate
      } else if (m_speed < getMaxSpeed()) {

        cout << "Accelerating!" << endl;
        m_speed += m_acceleration * (m_next_step_time / 1000000.0); // tr/s
      }
      
      if (m_speed <= 0.001) {
        return MAX_STEP_DELAY; // us
      }

      return ((unsigned long)(1000000.0 / (m_speed * m_steps_per_turn))); // us
    }*/

    virtual unsigned long calculateNextStepDelay(unsigned long timeSinceStart) {

      if (forceRotation) {return MAX_STEP_DELAY;}

      // Time to accelerate
      // Accelerate or go top speed
      if (timeSinceStart <= m_time_to_reach_middle_us) {
        
        m_speed = m_acceleration * (timeSinceStart / 1000000.0); // tr/s
        if (m_speed > m_max_speed) {m_speed = m_max_speed;} // tr/s

      // Decelerate
      } else if (timeSinceStart > m_time_to_start_decelerating_us) {

        //std::cout << "Decelerating.\n";
        double t_s = (timeSinceStart - m_time_to_start_decelerating_us) / 1000000.0; // s
        m_speed = m_max_speed_reached - (m_acceleration * t_s); // tr/s
      }

      if (m_speed <= 0.001) {
        return MAX_STEP_DELAY; // us
      }

      return ((unsigned long)(1000000.0 / (m_speed * m_steps_per_turn))); // us
    }

    void turnOneStep(unsigned long currentTime) {
      m_writer.doDigitalWrite(m_step_pin, m_is_step_high ? LOW : HIGH);
      m_is_step_high = !m_is_step_high;
      m_position_steps = m_position_steps + (isForward ? 1 : -1);

      m_next_step_time = currentTime + calculateNextStepDelay(currentTime); 
    }

    // Returns true if the axis is still working.
    virtual bool handleAxis(unsigned long currentTime) {
      //unsigned int delay = getDelay();
      
      if (isReferencing) {
        return moveToReference();
      } else if (canMove() && (forceRotation || !isDestinationReached())) {
        unsigned long timeSinceStart = timeDifference(m_start_time, currentTime); // us
        if (currentTime >= m_next_step_time) {
          turnOneStep(timeSinceStart);
        }
        return true;
      }
      return false;
    }

    // Resets some stuff.
    virtual void prepare(unsigned long time) {
      m_start_time = time;
      m_next_step_time = time;
      m_speed = 0;
      m_max_speed = m_default_max_speed;
    }

    bool m_is_step_high;

    long m_position_steps;

    double m_max_speed_reached;
    unsigned long m_next_step_time; // us
    unsigned long m_start_time; // us
    unsigned long m_time_to_reach_middle_us; // us
    unsigned long m_time_to_start_decelerating_us; // us
   
    double m_acceleration; // tour/s^2 [turn/sec^2]
    double m_max_speed; // tour/s [turn/sec]
    double m_default_max_speed; // tour/s [turn/sec]
    double m_speed;



    // TODO:
    // bool m_jog_forward; No more forceRotation.
    // bool m_jog_reverse; No more isForward.
};

class BaseXAxis : public StepperMotor {
  public:
    BaseXAxis(Writer& writer, char name, Axis& thetaAxis) : StepperMotor(writer, name), m_theta_axis(thetaAxis) {
    }
    
    /*void prepare(unsigned long time) {
      MotorAxis::prepare();
      m_start_time = time;
    }*/

    /*unsigned long calculateNextStepDelay(unsigned long timeSinceStart) {
      // Maybe use my own time because the theta position directly could get out of sync
      if (m_theta_axis.isDestinationReached()) {return MAX_STEP_DELAY;}
      double angularSpeedRad = m_theta_axis.getCurrentSpeed() / 180 * PI;
      double angle = m_theta_axis.getPosition();
      double vx = RAYON * angularSpeedRad * sind(angle);

      if (vx <= 0.001) {
        return MAX_STEP_DELAY; // us
      }

      double frequency = vx * stepsPerUnit;
      unsigned long delay = 1000000 / frequency;
      cout << "delay: " << delay << endl;

      if (delay > MAX_STEP_DELAY) {
        return MAX_STEP_DELAY;
      }

      return 1000000 / frequency;
    }*/

  private:
    //unsigned long m_start_time;
    Axis& m_theta_axis;
};

#endif
