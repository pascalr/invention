#ifndef AXIS_H
#define AXIS_H

#define RAYON 380

#define CW true
#define CCW false

#define NB_AXES 3

#ifndef LOW
#define LOW 0
#define HIGH 1
#define INPUT 0
#define OUTPUT 1
#endif

#define AXIS(t) (axisByLetter(axes, t))

class Writer {
  public:
    virtual void doPinMode(int pin, bool type) = 0;
    virtual void doDigitalWrite(int pin, bool value) = 0;
    virtual double doDigitalRead(int pin) = 0;
    virtual void doPrint(const char* theString) = 0;
    virtual void doPrintLn(const char* theString) = 0;
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
      isClockwise = false;
      isReferenced = false;
      isReferencing = false;
      speed = 500;
      forceRotation = false;
      maxPosition = 999999;
      m_position_steps = 0;
      m_destination_steps = 0;
    }

    void setRatio(double ratio) {
      m_ratio = ratio;
      stepsPerUnit = ratio;
    }

    double getSpeed() {
      double frequency = 1 / getDelay() * 1000000;
      double theSpeed = frequency / m_ratio;
      return theSpeed;
    }

    void setupPins() {
      m_writer->doPinMode(stepPin, OUTPUT);
      m_writer->doPinMode(dirPin, OUTPUT);
      m_writer->doPinMode(enabledPin, OUTPUT);
    }

    void rotate(bool direction) {
      setMotorDirection(direction);
      forceRotation = true;
      setMotorEnabled(true);
    }

    double getPosition() {
      return m_position_steps / stepsPerUnit;
    }

    double getDestination() {
      return destination;
    }

    void stop() {
      //setMotorsEnabled(false);
      setDestination(getPosition());
      forceRotation = false;
    }

    void turnOneStep() {
      m_writer->doDigitalWrite(stepPin, isStepHigh ? LOW : HIGH);
      isStepHigh = !isStepHigh;
      m_position_steps = m_position_steps + (isClockwise ? 1 : -1);
    }

    void setPositionSteps(double posSteps) {
      m_position_steps = posSteps;
    }

    void setDestination(double dest) {
      
      destination = dest;
      if (destination > maxPosition) {destination = maxPosition;}
      m_destination_steps = dest * stepsPerUnit;
      setMotorEnabled(true);
      setMotorDirection(m_destination_steps > m_position_steps);
    }
    
    void setMotorEnabled(bool value) {
      m_writer->doDigitalWrite(enabledPin, LOW); // FIXME: ALWAYS ENABLED
      //digitalWrite(enabledPin, value ? LOW : HIGH);
      isMotorEnabled = value;
    }

    void setMotorDirection(bool clockwise) {
      m_writer->doDigitalWrite(dirPin, clockwise ? LOW : HIGH);
      isClockwise = clockwise;
    }

    void startReferencing() {
      isReferencing = true;
      setMotorDirection(CCW);
      setMotorEnabled(true);
    }

    void referenceReached() {
      m_writer->doPrint("Done referencing axis ");
      char theName[] = {name, '\0'};
      m_writer->doPrintLn(theName);
      setPositionSteps(0);
      setDestination(0);
      setMotorEnabled(false);
      isReferenced = true;
      isReferencing = false;
    }

    // Only the vertical axis moves in order to do a reference
    void moveToReference() {
      referenceReached();
    }

    void handleAxis(unsigned long currentTime) {
      unsigned int delay = getDelay();
      if (isReferencing) {
        moveToReference();
      } else if (isReferenced && isMotorEnabled && currentTime - previousStepTime > delay && (forceRotation ||
                ((isClockwise && m_position_steps < m_destination_steps) || (!isClockwise && m_position_steps > m_destination_steps)))) {
        turnOneStep();
        if (currentTime - previousStepTime > 2*delay) {
          previousStepTime = currentTime; // refreshing previousStepTime when it is the first step and the motor was at a stop
        } else {
          previousStepTime = previousStepTime + delay; // This is more accurate to ensure all the motors are synchronysed
        }
      }
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
    bool isClockwise;
    bool isReferenced;
    bool isReferencing;
    bool forceRotation;
    char name;

    // Get the delay untill the next step
    double getDelay() {
      // TODO: The speed is not fixed.
      return speed;
    }
    
  protected:
    Writer* m_writer;

    unsigned long m_position_steps;
    unsigned long m_destination_steps;

    double m_ratio;
    
};

// The horizontal axis adjusts it's speed to compensate the rotary axis
class HorizontalAxis: public Axis {
  public:
    HorizontalAxis(Writer* theWriter, char theName) : Axis(theWriter, theName) {
      
    }

    void setRotationAxis(Axis* axis) {
      m_rotation_axis = axis;
    }
  
    unsigned int getDelay() {
      double theta = m_rotation_axis->getPosition();
      // Vx = r* W * sin(theta)
      // TODO: The speed is not fixed.
      return speed;
    }

  private:
    Axis* m_rotation_axis;
};

class VerticalAxis: public Axis {
  public:
    VerticalAxis(Writer* theWriter, char theName) : Axis(theWriter, theName) {
      
    }

    /*void moveToReference() {
      //Serial.println(digitalRead(axis.limitSwitchPin));
      if (!digitalRead(limitSwitchPin)) {
        axis->referenceReached();
      } else {
        turnOneStep();
        delayMicroseconds(SLOW_SPEED_DELAY);
      }
    }*/
};

#endif
