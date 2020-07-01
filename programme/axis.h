#ifndef AXIS_H
#define AXIS_H

#define RAYON 340.0

#define FORWARD true
#define REVERSE false

#include "writer.h"

#ifndef LOW
#define LOW 0
#define HIGH 1
#define INPUT 0
#define OUTPUT 1
#define PI 3.1415926535897932384626433832795
#endif

#define AXIS(t) (axisByLetter(axes, t))

// Y0 is on the floor
// X0 is on the left
// Z0 is aligned with the wheels that go up and down
// The arm starts at theta 0 degree. Counterclockwise is forward.

class Axis {
  public:
  
    Axis(Writer& writer, char theName) : m_writer(writer) {
      name = theName;
      destination = -1;
      previousStepTime = 0;
      isStepHigh = false;
      isMotorEnabled = false;
      isForward = false;
      isReferenced = false;
      isReferencing = false;
      speed = 500;
      forceRotation = false;
      maxPosition = 999999;
      m_position_steps = 0;
      //m_destination_steps = 0;
      m_reverse_motor_direction = false;
      m_min_position = 0;
    }

    void setReverseMotorDirection(bool val) {
      m_reverse_motor_direction = val;
    }

    virtual double getMaxPosition() {
      return maxPosition;
    }

    virtual void setMaxPosition(double maxP) {
      maxPosition = maxP;
    }

    virtual void serialize() {
      m_writer << "-Pos " << name << ": " << getPosition() << "\n";
      m_writer << "-Dest " << name << ": " << getDestination() << "\n";
      m_writer << "-Speed " << name << ": " << speed << "\n";
      m_writer << "-CW " << name << ": " << isForward << "\n";
      m_writer << "-Referenced " << name << ": " << isReferenced << "\n";
      m_writer << "-Referencing " << name << ": " << isReferencing << "\n";
      m_writer << "-Enabled " << name << ": " << isMotorEnabled << "\n";
      m_writer << "-Step " << name << ": " << isStepHigh << "\n";
      m_writer << "-Force " << name << ": " << forceRotation << "\n";
      m_writer << "-PIN enabled " << name << ": " << enabledPin << "\n";
      m_writer << "-PIN dir " << name << ": " << dirPin << "\n";
      m_writer << "-PIN step " << name << ": " << stepPin << "\n";
      m_writer << "-PIN limit switch " << name << ": " << limitSwitchPin << "\n";
      m_writer << "-stepsPerUnit " << name << ": " << stepsPerUnit << "\n";
      m_writer << "-m_position_steps " << name << ": " << m_position_steps << "\n";
      //m_writer << "-m_destination_steps " << name << ": " << m_destination_steps << "\n";
      m_writer << "-maxPosition " << name << ": " << maxPosition << "\n";
      m_writer << "-previousStepTime " << name << ": " << previousStepTime << "\n";
    }

    // Linear axes units are mm. Rotary axes units are degrees.
    // Number of steps per turn of the motor * microstepping / distance per turn
    // The value is multiplied by two because we have to write LOW then HIGH for one step
    virtual void setStepsPerUnit(double ratio) {
      stepsPerUnit = ratio;
    }

    virtual double getStepsPerUnit() {
      return stepsPerUnit;
    }

    virtual double getSpeed() {
      double frequency = 1 / getDelay() * 1000000;
      double theSpeed = frequency / stepsPerUnit;
      return theSpeed;
    }

    virtual void setupPins() {
      m_writer.doPinMode(stepPin, OUTPUT);
      m_writer.doPinMode(dirPin, OUTPUT);
      m_writer.doPinMode(enabledPin, OUTPUT);
    }

    virtual void rotate(bool direction) {
      setMotorDirection(direction);
      forceRotation = true;
      setMotorEnabled(true);
    }

    virtual bool notGoingOutOfBounds() {
      double p = getPosition();
      return isForward ? p <= getMaxPosition() : p >= m_min_position;
    }

    virtual bool canMove() {
      return isReferenced && isMotorEnabled && notGoingOutOfBounds();
    }

    virtual double getPosition() {
      return m_position_steps / stepsPerUnit;
    }

    virtual double getDestination() {
      return destination;
    }

    //virtual double getDestinationSteps() {
    //  return m_destination_steps;
    //}

    virtual void stop() {
      //setMotorsEnabled(false);
      setDestination(getPosition());
      forceRotation = false;
    }

    virtual void turnOneStep() {
      m_writer.doDigitalWrite(stepPin, isStepHigh ? LOW : HIGH);
      isStepHigh = !isStepHigh;
      m_position_steps = m_position_steps + (isForward ? 1 : -1);
    }

    virtual void setPositionSteps(double posSteps) {
      m_position_steps = posSteps;
    }

    virtual double getPositionSteps() {
      return m_position_steps;
    }

    virtual void updateDirection() {
      //std::cout << "Destination steps " << getDestinationSteps() << std::endl;
      //std::cout << "Position steps " << getPositionSteps() << std::endl;
      //setMotorDirection(getDestinationSteps() > getPositionSteps());
      setMotorDirection(getDestination() > getPosition());
    }

    virtual void setDestination(double dest) {
      
      destination = dest;
      if (destination > maxPosition) {destination = maxPosition;}
      if (destination < 0) {destination = 0;}
      //m_destination_steps = dest * stepsPerUnit;
      updateDirection();
    }
    
    virtual void setMotorEnabled(bool value) {
      m_writer.doDigitalWrite(m_reverse_motor_direction ? !enabledPin : enabledPin, LOW); // FIXME: ALWAYS ENABLED
      //digitalWrite(enabledPin, value ? LOW : HIGH);
      isMotorEnabled = value;
    }

    virtual void setMotorDirection(bool forward) {
      // Maybe add a variable to the axis: isClockwiseForward?
      m_writer.doDigitalWrite(dirPin, forward ? LOW : HIGH);
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
      m_writer.doPrint("Done referencing axis ");
      char theName[] = {name, '\0'};
      m_writer.doPrint(theName);
      m_writer.doPrint("\n");
      setPositionSteps(0);
      setDestination(0);
      setMotorEnabled(true);
      isReferenced = true;
      isReferencing = false;
    }

    // Only the vertical axis moves in order to do a reference
    virtual bool moveToReference() {
      referenceReached();
    }

    virtual void afterInput() {
    }

    // Returns true if the axis is still working.
    virtual bool handleAxis(unsigned long currentTime) {
      unsigned int delay = getDelay();
      
      if (isReferencing) {
        return moveToReference();
      } else if (canMove() && (forceRotation || !isDestinationReached())) {
        unsigned long deltaTime = currentTime - previousStepTime;
        if (deltaTime > delay) {
          turnOneStep();
          // TODO: Instead of this, call a function named prepareToMove, that updates the previousStepTime
          if (deltaTime > 2*delay) {
            previousStepTime = currentTime; // refreshing previousStepTime when it is the first step and the motor was at a stop
          } else {
            previousStepTime = previousStepTime + delay; // This is more accurate to ensure all the motors are synchronysed
          }
        }
        return true;
      }
      return false;
    }
  
  //protected:
    // Linear axes units are mm. Rotary axes units are degrees.
    double destination; // mm or degrees
    double maxPosition; // mm or degrees
    double speed; // delay in microseconds
    double stepsPerUnit;
    
    int enabledPin;
    int dirPin;
    int stepPin;
    int limitSwitchPin;
    
    unsigned long previousStepTime;
    
    bool isStepHigh;
    bool isMotorEnabled;
    bool isForward;
    bool isReferenced;
    bool isReferencing;
    bool forceRotation;
    char name;

    // Get the delay untill the next step
    virtual double getDelay() {
      // TODO: The speed is not fixed.
      return speed;
    }

    virtual void setFollowingAxis(Axis* axis) {
      m_following_axis = axis;
    }

    // Resets some stuff.
    virtual void prepare(unsigned long time) {
      m_following_axis = NULL;
      previousStepTime = time;
    }
    
  protected:
    Writer& m_writer;

    Axis* m_following_axis;

    long m_position_steps;

    bool m_reverse_motor_direction;
    double m_min_position;
    //double m_destination_steps;
};

// I DO MX0 at reference: I WANT:
// The x axis to do nothing at first because it is already at 0.
// The z axis to flip because it knows it has to.
// The z axis moves the delta x.
// The x axis compensates.
// axisX.setDestination(0); Does nothing, already at 0.
// axisZ.afterInput();
// axisZ.setDestinationAngle(180);
// moves Z
// x follows

// Destination refers to the tip
// Position refers to the tip
// Delta is the difference between the tip and the base
// Base = Tip - Delta
// The positionSteps refers to the base, these must start at zero
class HorizontalAxis : public Axis {
  public:
    HorizontalAxis(Writer& theWriter, char theName) : Axis(theWriter, theName) {
    }

    void setDeltaPosition(double pos) {
      m_delta_position = pos;
      updateDirection();
    }

    double getDeltaPosition() {
      return m_delta_position;
    }

    double getPosition() {
      return Axis::getPosition() + m_delta_position;
    }

    bool baseNotGoingOutOfBounds() {
      double p = Axis::getPosition();
      return isForward ? p <= getMaxPosition() : p >= m_min_position;
    }

    virtual bool canMove() {
      return Axis::canMove() && baseNotGoingOutOfBounds();
    }
    //double getPositionSteps() {
    //  return Axis::getPositionSteps() + m_delta_position * stepsPerUnit;
    //}

    //double getDestinationSteps() { TODO: Remove this function everywhere
    //  return Axis::getDestinationSteps() + m_delta_position * stepsPerUnit;
    //}

    /*void setDestination(double dest) {
      Axis::setDestination(dest);
      te
    }*/

    virtual void referenceReached() {
      Axis::referenceReached();
      m_delta_position = RAYON;
      setDestination(RAYON);
      //setPositionSteps(RAYON * stepsPerUnit);
    }

    virtual void serialize() {
      Axis::serialize();
      m_writer << "-DeltaPos " << name << ": " << m_delta_position << "\n";
    }
    
  private:
    // The horizontal distance between the tip and the base.
    double m_delta_position;
};

// The ZAxis is the TAxis.
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

    virtual void afterInput() {
      //std::cout << "getDestination" << std::endl;
      //std::cout << m_horizontal_axis->getDestination() << std::endl;
      // If a flip is required, do one
      if(m_horizontal_axis->getDestination() < RAYON && m_destination_angle < 90) {
        m_destination_angle = 180 - m_destination_angle;
        updateDirection();
      }
    }

    double getDestinationAngle() {
      return m_destination_angle;
    }

    virtual void serialize() {
      Axis::serialize();
      m_writer << "-DestAngle " << name << ": " << m_destination_angle << "\n";
    }
  private:
    HorizontalAxis* m_horizontal_axis;
    double m_destination_angle;
    bool m_is_clockwise;
};

class VerticalAxis: public Axis {
  public:
    VerticalAxis(Writer& theWriter, char theName) : Axis(theWriter, theName) {
      
    }
};

#endif
