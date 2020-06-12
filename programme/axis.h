#ifndef AXIS_H
#define AXIS_H

#define RAYON 380.0

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
      m_destination_steps = 0;
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
      m_writer << "-m_destination_steps " << name << ": " << m_destination_steps << "\n";
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

    virtual double getPosition() {
      return m_position_steps / stepsPerUnit;
    }

    virtual double getDestination() {
      return destination;
    }

    virtual double getDestinationSteps() {
      return m_destination_steps;
    }

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

    void updateDirection() {
      //std::cout << "Destination steps " << getDestinationSteps() << std::endl;
      //std::cout << "Position steps " << getPositionSteps() << std::endl;
      setMotorDirection(getDestinationSteps() > getPositionSteps());
    }

    virtual void setDestination(double dest) {
      
      destination = dest;
      if (destination > maxPosition) {destination = maxPosition;}
      if (destination < 0) {destination = 0;}
      m_destination_steps = dest * stepsPerUnit;
      updateDirection();
    }
    
    virtual void setMotorEnabled(bool value) {
      m_writer.doDigitalWrite(enabledPin, LOW); // FIXME: ALWAYS ENABLED
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
      double destSteps = getDestinationSteps();
      return (isForward && m_position_steps >= destSteps) ||
             (!isForward && m_position_steps <= destSteps);
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

    // Returns true if the axis is still working.
    virtual bool handleAxis(unsigned long currentTime) {
      unsigned int delay = getDelay();
      
      if (isReferencing) {
        return moveToReference();
      } else if (isReferenced && isMotorEnabled && (forceRotation || !isDestinationReached())) {
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

    unsigned long m_position_steps;
    double m_destination_steps;
};

class HorizontalAxis : public Axis {
  public:
    HorizontalAxis(Writer& theWriter, char theName) : Axis(theWriter, theName) {
      m_delta_destination = 0;
    }

    bool shouldGoForward() {
      return getPosition() < maxPosition / 2;
    }

    double getDestinationSteps() {
      return Axis::getDestinationSteps() + m_delta_destination * stepsPerUnit;
    }

    double getDestination() {
      return Axis::getDestination() + m_delta_destination;
    }
    
    void setDeltaDestination(double dest) {
      m_delta_destination = dest;
      updateDirection();
    }

    double getDeltaDestination() {
      return m_delta_destination;
    }

    virtual void prepare(unsigned long time) {
      Axis::prepare(time);
      m_delta_destination = 0;
    }

    virtual void serialize() {
      Axis::serialize();
      m_writer << "-DeltaDest " << name << ": " << m_delta_destination << "\n";
    }
    
  private:
    double m_delta_destination;
};

// The ZAxis is the TAxis.
class ZAxis : public Axis {
  public:
    ZAxis(Writer& theWriter, char theName, HorizontalAxis* hAxis) : Axis(theWriter, theName) {
      m_horizontal_axis = hAxis;
    }

    virtual void turnOneStep() {
      Axis::turnOneStep();
      double deltaX = (getPosition() - m_original_position) * (m_horizontal_axis->shouldGoForward() ? 1 : -1);
      ////std::cout << "position " << getPosition() << std::endl;
      m_horizontal_axis->setDeltaDestination(deltaX);
      ////std::cout << "2" << std::endl;
    }

/*void followedAxisMoved(double oldPosition, double position, double followedStepsPerUnit) {
-
-      //setDestination(getDestination() + position - oldPosition);
-
-      //double deltaAngle = (position - oldPosition);// * stepsPerUnit / followedStepsPerUnit;
-
-      // OPTIMIZE: This can probably be done with only one cos.
-      double deltaX = (RAYON * cos(m_rotation_axis_beginning_position * PI / 180)) - (RAYON * cos(position * PI / 180));
-      m_rotation_position = deltaX * ((int)m_should_go_forward);
-      //setDestination(getDestination() + (deltaX * (int)m_should_go_forward));
-    }*/

    virtual double getPosition() {
      double angle = m_position_steps / stepsPerUnit;
      ////std::cout << "m_position_steps " << m_position_steps << std::endl;
      return RAYON * sin(angle / 180 * PI);
    }

    double getDestinationSteps() {
      return m_destination_angle * stepsPerUnit;
    }

    void setDestination(double dest) {
      //std::cout << "Set destination " << dest << std::endl;
      //std::cout << "Is forward " << isForward << std::endl;
      
      m_original_position = getPosition();
      m_destination_angle = asin(dest / RAYON) * 180.0 / PI;
      Axis::setDestination(dest);
      //std::cout << "Is forward " << isForward << std::endl;
      
    }

    virtual void serialize() {
      Axis::serialize();
      m_writer << "-DestAngle " << name << ": " << m_destination_angle << "\n";
      m_writer << "-OriginalPos " << name << ": " << m_original_position << "\n";
    }
  private:
    HorizontalAxis* m_horizontal_axis;
    double m_destination_angle;
    double m_original_position;
};

class VerticalAxis: public Axis {
  public:
    VerticalAxis(Writer& theWriter, char theName) : Axis(theWriter, theName) {
      
    }
};

#endif
