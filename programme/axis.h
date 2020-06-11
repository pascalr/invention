#ifndef AXIS_H
#define AXIS_H

#define RAYON 380.0

#define FORWARD true
#define REVERSE false

#ifndef LOW
#define LOW 0
#define HIGH 1
#define INPUT 0
#define OUTPUT 1
#define PI 3.1415926535897932384626433832795
#endif

#define AXIS(t) (axisByLetter(axes, t))

class Writer {
  public:
    virtual void doPinMode(int pin, bool type) = 0;
    virtual void doDigitalWrite(int pin, bool value) = 0;
    virtual double doDigitalRead(int pin) = 0;
    virtual void doPrint(const char* theString) = 0;
    virtual void doPrint(char val) = 0;
    virtual void doPrintLn(const char* theString) = 0;
    virtual void doPrintLn(double val) = 0;
};

class Axis {
  public:
  
    Axis(Writer* writer, char theName) {
      m_writer = writer;
      name = theName;
      destination = -1;
      previousStepTime = 0;
      isStepHigh = false;
      isMotorEnabled = false;
      isForward = true;
      isReferenced = false;
      isReferencing = false;
      speed = 500;
      forceRotation = false;
      maxPosition = 999999;
      m_position_steps = 0;
      m_destination_steps = 0;
      m_is_working = false;
    }

    virtual void setMaxPosition(double maxP) {
      maxPosition = maxP;
    }

    virtual bool isWorking() {
      return m_is_working;
    }

    virtual void setIsWorking(bool val) {
      m_is_working = val;
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
      m_writer->doPinMode(stepPin, OUTPUT);
      m_writer->doPinMode(dirPin, OUTPUT);
      m_writer->doPinMode(enabledPin, OUTPUT);
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
      m_writer->doDigitalWrite(stepPin, isStepHigh ? LOW : HIGH);
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
      m_writer->doDigitalWrite(enabledPin, LOW); // FIXME: ALWAYS ENABLED
      //digitalWrite(enabledPin, value ? LOW : HIGH);
      isMotorEnabled = value;
    }

    virtual void setMotorDirection(bool clockwise) {
      m_writer->doDigitalWrite(dirPin, clockwise ? LOW : HIGH);
      isForward = clockwise;
    }

    virtual void startReferencing() {
      referenceReached(); // FIMXE: Temporaty
      
      /*isReferencing = true;
      setMotorDirection(CCW);
      setMotorEnabled(true);*/
    }

    virtual void referenceReached() {
      m_writer->doPrint("Done referencing axis ");
      char theName[] = {name, '\0'};
      m_writer->doPrintLn(theName);
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
      double destSteps = getDestinationSteps();
      if (isReferencing) {
        return moveToReference();
      } else if (isReferenced && isMotorEnabled && (forceRotation ||
                ((isForward && m_position_steps < destSteps) ||
                (!isForward && m_position_steps > destSteps)))) {
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
    Writer* m_writer;

    Axis* m_following_axis;

    unsigned long m_position_steps;
    unsigned long m_destination_steps;

    bool m_is_working; // FIXME: Probably useless
};

class HorizontalAxis : public Axis {
  public:
    HorizontalAxis(Writer* theWriter, char theName) : Axis(theWriter, theName) {
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
  private:
    double m_delta_destination;
};

// The ZAxis is the TAxis.
class ZAxis : public Axis {
  public:
    ZAxis(Writer* theWriter, char theName, HorizontalAxis* hAxis) : Axis(theWriter, theName) {
      m_horizontal_axis = hAxis;
    }

    virtual void turnOneStep() {
      Axis::turnOneStep();
      double deltaX = (getPosition() - m_original_position) * (m_horizontal_axis->shouldGoForward() ? -1 : 1);
      m_horizontal_axis->setDeltaDestination(deltaX);
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
      return RAYON * sin(angle / 180 * PI);
    }

    double getDestinationSteps() {
      return m_destination_angle * stepsPerUnit;
    }

    void setDestination(double dest) {
      m_original_position = getPosition();
      Axis::setDestination(dest);
      m_destination_angle = asin(dest / RAYON) * 180.0 / PI;
    }
  private:
    HorizontalAxis* m_horizontal_axis;
    double m_destination_angle;
    double m_original_position;
};

class VerticalAxis: public Axis {
  public:
    VerticalAxis(Writer* theWriter, char theName) : Axis(theWriter, theName) {
      
    }
};

#endif
