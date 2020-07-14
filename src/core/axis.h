#ifndef AXIS_H
#define AXIS_H

#include "writer.h"
#include "../config/constants.h"
#include <math.h>

#define AXIS(t) (axisByLetter(axes, t))

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

      if (dest >= m_max_position + 0.0000001) {return ERROR_DESTINATION_TOO_HIGH;}
      if (dest <= m_min_position - 0.0000001) {return ERROR_DESTINATION_TOO_LOW;}

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
      isStepHigh = false;
      isMotorEnabled = false;
      isReferenced = false;
      isReferencing = false;
      speed = 500;
      forceRotation = false;
      m_position_steps = 0;
      m_reverse_motor_direction = false;
      m_min_position = 0;
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

    // Returns true if the axis is still working.
    virtual bool handleAxis(unsigned long currentTime) {
      unsigned int delay = getDelay();
      
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
};

/*// The ZAxis is the TAxis.
class ZAxis : public Axis {
  public:
    ZAxis(Writer& theWriter, char theName, HorizontalAxis* hAxis) : Axis(theWriter, theName) {
      m_horizontal_axis = hAxis;
    }

    // It is not about if the horizontal axis should go forward or not,
    // it is about whether the T axis should turn clockwise or counter clockwise to get to the z position.
    // The x axis just follows.

    virtual void turnOneStep() {
      Axis::turnOneStep();
      double angle = m_position_steps / stepsPerUnit;
      double deltaX = RAYON * cos(angle / 180 * PI);
      m_horizontal_axis->setDeltaPosition(deltaX);
    }

    virtual double getPositionAngle() {
      return m_position_steps / stepsPerUnit;
    }

    virtual double getPosition() {
      return RAYON * sin(getPositionAngle() / 180 * PI);
    }

    virtual bool isDestinationReached() {
      //double destSteps = getDestinationSteps();
      //double posSteps = getPositionSteps();
      //return (isForward && posSteps >= destSteps) ||
      //       (!isForward && posSteps <= destSteps);
      return (isForward && getPositionAngle() >= getDestinationAngle()) ||
             (!isForward && getPositionAngle() <= getDestinationAngle());
    }

    //double getDestinationSteps() {
    //  return m_destination_angle * stepsPerUnit;
    //}

    void setDestination(double dest) {
      //std::cout << "Set destination " << dest << std::endl;
      //std::cout << "Is forward " << isForward << std::endl;

      m_destination_angle = (asin(dest / RAYON) * 180.0 / PI);

      Axis::setDestination(dest);
      //std::cout << "destination angle : " << m_destination_angle << std::endl;
      //std::cout << "Is forward " << isForward << std::endl;
      
    }

    virtual void updateDirection() {
      setMotorDirection(getDestinationAngle() > getPositionAngle());
    }

    

    double getDestinationAngle() {
      return m_destination_angle;
    }

  private:
    HorizontalAxis* m_horizontal_axis;
    double m_destination_angle;
    bool m_is_clockwise;
};*/

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

    virtual double getDelay() {
      // Maybe use my own time because the theta position directly could get out of sync
      if (m_theta_axis.isDestinationReached()) {return speed;}
      double angularSpeedRad = m_theta_axis.getSpeed() / 180 * PI;
      double angle = m_theta_axis.getPosition();
      double vx = RAYON * angularSpeedRad * sind(angle);
      double frequency = vx * stepsPerUnit;
      if (frequency == 0) return 999999999;
      return 1000000 / frequency;
    }

  private:
    //unsigned long m_start_time;
    MotorAxis& m_theta_axis;
};

Axis* axisByLetter(Axis** axes, char letter);

#endif
