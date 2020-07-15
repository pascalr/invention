#ifndef AXIS_H
#define AXIS_H

#include "writer.h"
#include "../config/constants.h"
#include <math.h>

#define AXIS(t) (axisByLetter(axes, t))

#define MAX_STEP_DELAY 50000 // us (50 ms)

#ifndef ARDUINO
#include <iostream>
using namespace std;
#endif

// I would like to transform the mouvement asked into an optimal path with optimal speed for all axis.
// If I the arm is at z0 and I ask mz340, I expect it to be in a straight line.
// The speed of the x axis will be constant (easier for now) v
// The speed of the theta axis would be (for theta=0 to theta=90) cos(theta)
// The axis all synchronized with the time, so it's ok.
// If they lag behing they will try to go a little faster.

// Y0 is on the floor
// X0 is on the left
// Z0 is aligned with the wheels that go up and down
// The arm starts at theta 0 degree. Counterclockwise is forward.

// TODO: A simple axis has a destination and a position.
// Add a class MotorAxis
// A motor axis has pins and all the stuff.
//
// You cannot go forward or backward with x and z axis, because they are not MotorAxis

class Axis {
  public:
    Axis(Writer& writer, char name) : m_writer(writer) {
      m_name = name;
      m_destination = -1;
      m_max_position = 999999;
      m_min_position = 0;
    }

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

  protected:
    Writer& m_writer;
    char m_name;
    double m_destination;
    double m_max_position;
    double m_min_position;
};

class MotorAxis : public Axis {
  public:

    MotorAxis(Writer& writer, char theName) : Axis(writer, theName) {
      isForward = false;
      m_previous_step_time = 0;
      m_is_step_high = false;
      isMotorEnabled = false;
      isReferenced = false;
      isReferencing = false;
      speed = 500;
      forceRotation = false;
      m_position_steps = 0;
      m_reverse_motor_direction = false;

      m_max_speed = 1;
      m_acceleration = 0.2; // tour/s^2 [turn/sec^2]

    }

    void setAcceleration(double accel) {
      m_acceleration = accel;
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

    double getMaxSpeed() {
      return m_max_speed;
    }
    
    void setReverseMotorDirection(bool val) {
      m_reverse_motor_direction = val;
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

    

    void setupPins() {
      m_writer.doPinMode(stepPin, OUTPUT);
      m_writer.doPinMode(dirPin, OUTPUT);
      m_writer.doPinMode(enabledPin, OUTPUT);
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
      m_writer.doDigitalWrite(enabledPin, LOW); // FIXME: ALWAYS ENABLED
      //digitalWrite(enabledPin, value ? LOW : HIGH);
      isMotorEnabled = value;
    }

    void setMotorDirection(bool forward) {
      bool val = forward ? LOW : HIGH;
      val = m_reverse_motor_direction ? !val : val;
      m_writer.doDigitalWrite(dirPin, val);
      isForward = forward;
    }

    virtual void startReferencing() {
      referenceReached(); // FIMXE: Temporaty
      
      /*isReferencing = true;
      setMotorDirection(CCW);
      setMotorEnabled(true);*/
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
    double speed; // delay in microseconds
    double stepsPerUnit;
    double m_steps_per_turn;
    
    int enabledPin;
    int dirPin;
    int stepPin;
    int limitSwitchPin;
    
    bool isMotorEnabled;
    bool isForward;
    bool isReferenced;
    bool isReferencing;
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

    // TODO: All the units should simply be steps... NOOOOOOOOOO All units are in turns!
    // WARNING: THIS ONLY WORKS AT THE BEGINNING, DOES NOT COMPUTE ALL THE TIME
    unsigned long timeToReachDestinationUs() {
      return m_time_to_reach_middle_us * 2;
    }

    void calculateMovement(double dest) {

      double halfDistance = ((dest - getPosition()) / 2) * stepsPerUnit / m_steps_per_turn; // tr
      halfDistance *= (halfDistance < 0) ? -1 : 1;

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

      calculateMovement(dest);
      return 0;
    }

    unsigned long calculateNextStepDelay(unsigned long currentTime) {

      double speed; // tr/s
      unsigned long deltaTime = currentTime - m_start_time; // us
      double deltaTimeS = deltaTime / 1000000.0; // s

      // Accelerate or go top speed
      if (deltaTime <= m_time_to_reach_middle_us) {
        
        speed = m_acceleration * deltaTimeS; // tr/s
        if (speed > m_max_speed) {speed = m_max_speed;} // tr/s

      // Go top speed
      } else if (currentTime < m_time_to_start_decelerating_us) {

        //std::cout << "Top speed.\n";
        // I am worried that a step could be missed due to calculation imprecision
        // This is why I use m_max_speed_reached instead of m_max_speed.
        speed = m_max_speed_reached; // tr/s

      // Decelerate
      } else {

        //std::cout << "Decelerating.\n";
        double t_s = (currentTime - m_time_to_start_decelerating_us) / 1000000.0; // s
        speed = m_max_speed_reached - (m_acceleration * t_s); // tr/s
      }

      if (speed <= 0.001) {
        return MAX_STEP_DELAY; // us
      }

      return ((unsigned long)(1000000.0 / (speed * m_steps_per_turn))); // us
    }

    void turnOneStep(unsigned long currentTime) {
      m_writer.doDigitalWrite(stepPin, m_is_step_high ? LOW : HIGH);
      m_is_step_high = !m_is_step_high;
      m_position_steps = m_position_steps + (isForward ? 1 : -1);

      m_next_step_time = currentTime + calculateNextStepDelay(currentTime); 
    }

    // Returns true if the axis is still working.
    virtual bool handleAxis(unsigned long currentTime) {
      //unsigned int delay = getDelay();
      
      //if (currentTime % 1000000 == 0) {
        //cout << "A second passed! \n";
      //}
      
      if (isReferencing) {
        return moveToReference();
      } else if (canMove() && (forceRotation || !isDestinationReached())) {
        if (currentTime >= m_next_step_time) {
          turnOneStep(currentTime);
        }
        return true;
      }
      return false;
    }

    // Resets some stuff.
    virtual void prepare(unsigned long time) {
      m_start_time = time;
      m_previous_step_time = time;
      m_next_step_time = time;
    }

    bool m_is_step_high;

    unsigned long m_previous_step_time;
    long m_position_steps;
    bool m_reverse_motor_direction;

    double m_max_speed_reached;
    unsigned long m_next_step_time; // us
    unsigned long m_start_time; // us
    unsigned long m_time_to_reach_middle_us; // us
    unsigned long m_time_to_start_decelerating_us; // us
   
    double m_acceleration; // tour/s^2 [turn/sec^2]
    double m_max_speed; // tour/s [turn/sec]



    // TODO:
    // bool m_jog_forward; No more forceRotation.
    // bool m_jog_reverse; No more isForward.
};

class XAxis : public Axis {
  public:
    XAxis(Writer& writer, char name, Axis& baseXAxis, Axis& thetaAxis) : Axis(writer, name), m_base_x_axis(baseXAxis), m_theta_axis(thetaAxis) {
    }

    double getPosition() {
      return m_base_x_axis.getPosition() + RAYON * cosd(m_theta_axis.getPosition());
    }
    
    void prepare(unsigned long time) {
      setDestination(getPosition());
    }

  private:
    Axis& m_base_x_axis;
    Axis& m_theta_axis;
};

class ZAxis : public Axis {
  public:
    ZAxis(Writer& writer, char name, Axis& thetaAxis) : Axis(writer, name), m_theta_axis(thetaAxis) {
    }

    double getPosition() {
      return RAYON * sind(m_theta_axis.getPosition());
    }

    void prepare(unsigned long time) {
      setDestination(getPosition());
    }
    
  private:
    Axis& m_theta_axis;
};

class BaseXAxis : public MotorAxis {
  public:
    BaseXAxis(Writer& writer, char name, MotorAxis& thetaAxis) : MotorAxis(writer, name), m_theta_axis(thetaAxis) {
    }
    
    /*void prepare(unsigned long time) {
      MotorAxis::prepare();
      m_start_time = time;
    }*/

    /*virtual double getDelay() {
      // Maybe use my own time because the theta position directly could get out of sync
      if (m_theta_axis.isDestinationReached()) {return speed;}
      double angularSpeedRad = m_theta_axis.getSpeed() / 180 * PI;
      double angle = m_theta_axis.getPosition();
      double vx = RAYON * angularSpeedRad * sind(angle);
      double frequency = vx * stepsPerUnit;
      if (frequency == 0) return 999999999;
      return 1000000 / frequency;
    }*/

  private:
    //unsigned long m_start_time;
    MotorAxis& m_theta_axis;
};

Axis* axisByLetter(Axis** axes, char letter);

#endif
