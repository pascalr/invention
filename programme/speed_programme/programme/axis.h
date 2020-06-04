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
      position = -1;
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

    double getPositionUnit() {
      return position/stepsPerUnit;
    }

    void stop() {
      //setMotorsEnabled(false);
      destination = position;
      forceRotation = false;
    }

    void turnOneStep() {
      m_writer->doDigitalWrite(stepPin, isStepHigh ? LOW : HIGH);
      isStepHigh = !isStepHigh;
      position = position + (isClockwise ? 1 : -1);
    }

    void setDestinationUnit(unsigned long dest) {
      destination = dest * stepsPerUnit;
      if (destination > maxPosition) {destination = maxPosition;}
      setMotorEnabled(true);
      setMotorDirection(destination > position);
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
      position = 0;
      destination = 0;
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
                ((isClockwise && position < destination) || (!isClockwise && position > destination)))) {
        turnOneStep();
        if (currentTime - previousStepTime > 2*delay) {
          previousStepTime = currentTime; // refreshing previousStepTime when it is the first step and the motor was at a stop
        } else {
          previousStepTime = previousStepTime + delay; // This is more accurate to ensure all the motors are synchronysed
        }
      }
    }
  
  //protected:
    unsigned long position; // position in steps
    unsigned long destination; // destination in steps
    unsigned long maxPosition;  // max position in steps
    unsigned int speed; // speed in microseconds
    double stepsPerUnit; // Linear axes units are mm. Rotary axes units are degrees.
    
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
    unsigned int getDelay() {
      // TODO: The speed is not fixed.
      return speed;
    }
    
  protected:
    Writer* m_writer;
    
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
      double theta = m_rotation_axis->getPositionUnit();
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
        Serial.print("Done referencing axis ");
        Serial.println(name);
        position = 0;
        destination = 0;
        setMotorEnabled(false);
        isReferenced = true;
        isReferencing = false;
      } else {
        turnOneStep();
        delayMicroseconds(SLOW_SPEED_DELAY);
      }
    }*/
};

#endif
