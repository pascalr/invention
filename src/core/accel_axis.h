#ifndef ACCEL_AXIS_H
#define ACCEL_AXIS_H

#include <AccelStepper.h>

class AccelAxis : public Axis {
  public:

    AccelAxis(Writer& writer, char theName) : Axis(writer, theName) {
      /*isForward = false;
      m_previous_step_time = 0;
      isStepHigh = false;
      isMotorEnabled = false;
      isReferenced = false;
      isReferencing = false;
      speed = 500;
      forceRotation = false;
      m_position_steps = 0;
      m_reverse_motor_direction = false;
      m_min_position = 0;*/
    }

    void prepare(unsigned long time) {
      bool destinationReached 
    }
    
    int setDestination(double dest) {
      if (isReferenced == false) {return ERROR_AXIS_NOT_REFERENCED;}
      return Axis::setDestination(dest);
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

    double getStepsPerUnit() {
      return stepsPerUnit;
    }

    double getSpeed() {
      double frequency = 1 / getDelay() * 1000000;
      double theSpeed = frequency / stepsPerUnit;
      return theSpeed;
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

    void turnOneStep() {
      m_writer.doDigitalWrite(stepPin, isStepHigh ? LOW : HIGH);
      isStepHigh = !isStepHigh;
      m_position_steps = m_position_steps + (isForward ? 1 : -1);
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
    
    int enabledPin;
    int dirPin;
    int stepPin;
    int limitSwitchPin;
    
    
    bool isStepHigh;
    bool isMotorEnabled;
    bool isForward;
    bool isReferenced;
    bool isReferencing;
    bool forceRotation;



    // Get the delay untill the next step
    virtual double getDelay() {
      // TODO: The speed is not fixed.
      return speed;
    }

    //double m_destination_steps;

    void setDestination() {
    }

    // Returns true if the axis is still working.
    virtual bool handleAxis(unsigned long currentTime) {

      if (isReferencing) {
        return moveToReference();
      } else if (canMove() && forceRotation) {
        if (forceRotation) {
          stepper.run();
          return true;
        } else if (m_stepper.distanceToGo() > 0) {
          stepper.run();
          return true;
        }
      } else if (canMove() && ) {
      }

      if (m_stepper.distanceToGo() <= 0) {return false;}
      m_stepper.moveTo(getDestination());
      return true;
      
      if (isReferencing) {
        return moveToReference();
      } else if (canMove() && (forceRotation || !isDestinationReached())) {
        unsigned long deltaTime = currentTime - m_previous_step_time;
        if (deltaTime > delay) {
          turnOneStep();
          // TODO: Instead of this, call a function named prepareToMove, that updates the m_previous_step_time
          if (deltaTime > 2*delay) {
            m_previous_step_time = currentTime; // refreshing m_previous_step_time when it is the first step and the motor was at a stop
          } else {
            m_previous_step_time = m_previous_step_time + delay; // This is more accurate to ensure all the motors are synchronysed
          }
        }
        return true;
      }
      return false;
    }

    // Resets some stuff.
    virtual void prepare(unsigned long time) {
      m_previous_step_time = time;
    }
  protected:

    unsigned long m_previous_step_time;
    long m_position_steps;
    bool m_reverse_motor_direction;
    AccelStepper m_stepper;
};

#endif
