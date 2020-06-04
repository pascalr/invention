// OUTPUT PINS
#define ledPin 13

// CONSTANTS
#define SLOW_SPEED_DELAY 2000
#define FAST_SPEED_DELAY 100

#define CW true
#define CCW false

class Axis {
  public:
  
    Axis() {
    }

    void turnOneStep() {
      digitalWrite(stepPin, isStepHigh ? LOW : HIGH);
      isStepHigh = !isStepHigh;
      position = position + (isClockwise ? 1 : -1);
    }
    
    void setMotorEnabled(bool value) {
      digitalWrite(enabledPin, LOW); // FIXME: ALWAYS ENABLED
      //digitalWrite(enabledPin, value ? LOW : HIGH);
      isMotorEnabled = value;
    
      digitalWrite(ledPin, value ? HIGH : LOW);
    }

    void setMotorDirection(bool clockwise) {
      digitalWrite(dirPin, clockwise ? LOW : HIGH);
      isClockwise = clockwise;
      
      delayMicroseconds(SLOW_SPEED_DELAY);
    }

    void referenceReached() {
      Serial.print("Done referencing axis ");
      Serial.println(name);
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
      if (isReferencing) {
        moveToReference();
      } else if (isReferenced && isMotorEnabled && currentTime - previousStepTime > speed && (forceRotation ||
                ((isClockwise && position < destination) || (!isClockwise && position > destination)))) {
        turnOneStep();
        if (currentTime - previousStepTime > 2*speed) {
          previousStepTime = currentTime; // refreshing previousStepTime when it is the first step and the motor was at a stop
        } else {
          previousStepTime = previousStepTime + speed; // This is more accurate to ensure all the motors are synchronysed
        }
      }
    }
  
  //private:
    unsigned long position;
    unsigned long destination;
    unsigned long maxPosition;
    unsigned int speed;
    int enabledPin;
    int dirPin;
    int stepPin;
    int limitSwitchPin;
    unsigned long previousStepTime;
    double stepsPerUnit; // Linear axes units are mm. Rotary axes units are degrees.
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
};

// The horizontal axis adjusts it's speed to compensate the rotary axis
class HorizontalAxis: public Axis {
  public:
    HorizontalAxis() : Axis {} {
      
    }
  
  unsigned int getDelay() {
    // TODO: The speed is not fixed.
    return speed;
  }
};

class VerticalAxis: public Axis {
  public:
    VerticalAxis() : Axis {} {
      
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

Axis axisT, axisW, oups;
VerticalAxis axisY;
HorizontalAxis axisX;

Axis& axisByLetter(char letter) {
  if (letter == 'X' || letter == 'x') {
    return axisX;
  } else if (letter == 'Y' || letter == 'y') {
    return axisY;
  } else if (letter == 'Z' || letter == 'z') {
    return axisT;
  } else if (letter == 'W' || letter == 'w') {
    return axisW;
  } else {
    return oups;
  }
}

unsigned long currentTime;

void setupAxis(Axis& axis, char name, int speed) {
  axis.name = name;
  axis.position = -1;
  axis.destination = -1;
  axis.previousStepTime = micros();
  axis.isStepHigh = false;
  axis.isMotorEnabled = false;
  axis.isClockwise = false;
  axis.isReferenced = false;
  axis.isReferencing = false;
  axis.speed = speed;
  axis.forceRotation = false;
  
  pinMode(axis.stepPin, OUTPUT);
  pinMode(axis.dirPin, OUTPUT);
  pinMode(axis.enabledPin, OUTPUT);
}

void setup() {

  //Initiate Serial communication.
  Serial.begin(9600);
  Serial.println("Setup...");

  // ************* PIN LAYOUT **************
  pinMode(ledPin, OUTPUT);
  
  axisT.enabledPin = 8;
  axisT.dirPin = 10;
  axisT.stepPin = 11;
  axisT.limitSwitchPin = 12;

  axisX.enabledPin = 8;
  axisX.dirPin = 2;
  axisX.stepPin = 3;
  axisX.limitSwitchPin = 12;

  axisY.enabledPin = 8;
  axisY.dirPin = 7;
  axisY.stepPin = 6;
  axisY.limitSwitchPin = 12;
  // ***************************************

  setupAxis(axisX, 'X', 500);
  setupAxis(axisY, 'Y', 500);
  setupAxis(axisT, 'Z', 500);
  
  axisX.maxPosition = 999999;
  axisY.maxPosition = 999999;
  axisT.maxPosition = 999999;

  // Linear axes units are mm. Rotary axes units are degrees.
  // Number of steps per turn of the motor * microstepping / distance per turn
  axisX.stepsPerUnit = 200 * 16 / (1.25*25.4*3.1416);
  axisY.stepsPerUnit = 200 * 16 / (2.625*25.4*3.1416);
  axisT.stepsPerUnit = 200 * 16 / (360*12/61);
  
  axisX.setMotorEnabled(false);
  axisY.setMotorEnabled(false);
  axisT.setMotorEnabled(false);
  
  Serial.println("Done");
}

int numberLength(String str) {
  int i;
  for (i = 0; i < str.length(); i++) {
    if (str[i] < '0' || str[i] > '9') {break;}
  }
  return i;
}

void parseSpeed(String cmd) {
  for (int i = 0; i < cmd.length(); i++) {
    int nbLength = numberLength(cmd.substring(i+1));
    Axis& axis = axisByLetter(cmd[i]);
    axis.speed = cmd.substring(i+1,i+1+nbLength).toInt();
    i = i+nbLength;
  }
}

void parseMove(String cmd) {
  for (int i = 0; i < cmd.length(); i++) {
    int nbLength = numberLength(cmd.substring(i+1));
    Axis& axis = axisByLetter(cmd[i]);
    axis.destination = cmd.substring(i+1,i+1+nbLength).toInt() * axis.stepsPerUnit;
    if (axis.destination > axis.maxPosition) {axis.destination = axis.maxPosition;}
    axis.setMotorEnabled(true);
    axis.setMotorDirection(axis.destination > axis.position);
    i = i+nbLength;
  }
}

void loop() {
  currentTime = micros();

  if (Serial.available() > 0) {
    String input = Serial.readString();
    input.remove(input.length()-1);

    Serial.print("Cmd: ");
    Serial.println(input);
    if (input.charAt(0) == 'M' || input.charAt(0) == 'm') {
      parseMove(input.substring(1));
    } else if (input.charAt(0) == 'V' || input.charAt(0) == 'v') { // speed (eg. VX300 -> axis X speed 300 microseconds delay per step)
      parseSpeed(input.substring(1));
    } else if (input.charAt(0) == 's' || input.charAt(0) == 'S') { // stop
      //setMotorsEnabled(false);
      axisX.destination = axisX.position;
      axisY.destination = axisY.position;
      axisT.destination = axisT.position;
      axisW.destination = axisW.position;
      axisX.forceRotation = false;
      axisY.forceRotation = false;
      axisT.forceRotation = false;
      axisW.forceRotation = false;
    } else if (input.charAt(0) == 'H' || input.charAt(0) == 'h') { // home reference (eg. H, or HX, or HY, ...)
      Serial.println("Referencing...");
      if (input.length() == 1) {
        axisX.isReferencing = true;
        axisY.isReferencing = true;
        axisT.isReferencing = true;
        axisW.isReferencing = true;
        axisX.setMotorDirection(CCW);
        axisY.setMotorDirection(CCW);
        axisT.setMotorDirection(CCW);
        axisX.setMotorEnabled(true);
        axisY.setMotorEnabled(true);
        axisT.setMotorEnabled(true);
      } else {
        Axis& axis = axisByLetter(input.charAt(1));
        axis.isReferencing = true;
        axis.setMotorDirection(CCW);
        axis.setMotorEnabled(true);
      }
    } else if (input == "?") { // debug info
      printDebugInfo();
    } else if (input.charAt(0) == '+') {
      Axis& axis = axisByLetter(input.charAt(1));
      axis.setMotorDirection(CW);
      axis.forceRotation = true;
      axis.setMotorEnabled(true);
    } else if (input.charAt(0) == '-') {
      Axis& axis = axisByLetter(input.charAt(1));
      axis.setMotorDirection(CCW);
      axis.forceRotation = true;
      axis.setMotorEnabled(true);
    }
  }

  axisX.handleAxis(currentTime);
  axisY.handleAxis(currentTime);
  axisT.handleAxis(currentTime);
}

void printDebugInfo() {
  printDebugAxis(axisX);
  printDebugAxis(axisY);
  printDebugAxis(axisT);
}

void printDebugAxis(Axis& axis) {
  Serial.print("-Pos ");
  Serial.print(axis.name);
  Serial.print(": ");
  Serial.println(axis.position);
  
  Serial.print("-Dest ");
  Serial.print(axis.name);
  Serial.print(": ");
  Serial.println(axis.destination);
  
  Serial.print("-Speed ");
  Serial.print(axis.name);
  Serial.print(": ");
  Serial.println(axis.speed);

  Serial.print("-CW ");
  Serial.print(axis.name);
  Serial.print(": ");
  Serial.println(axis.isClockwise);
  
  Serial.print("-Referenced ");
  Serial.print(axis.name);
  Serial.print(": ");
  Serial.println(axis.isReferenced);
  
  Serial.print("-Referencing ");
  Serial.print(axis.name);
  Serial.print(": ");
  Serial.println(axis.isReferencing);
  
  Serial.print("-Enabled ");
  Serial.print(axis.name);
  Serial.print(": ");
  Serial.println(axis.isMotorEnabled);
  
  Serial.print("-Step ");
  Serial.print(axis.name);
  Serial.print(": ");
  Serial.println(axis.isStepHigh);

  Serial.print("-Force ");
  Serial.print(axis.name);
  Serial.print(": ");
  Serial.println(axis.forceRotation);

  Serial.print("-PIN enabled ");
  Serial.print(axis.name);
  Serial.print(": ");
  Serial.println(digitalRead(axis.enabledPin));
  
  Serial.print("-PIN dir ");
  Serial.print(axis.name);
  Serial.print(": ");
  Serial.println(digitalRead(axis.dirPin));
  
  Serial.print("-PIN step ");
  Serial.print(axis.name);
  Serial.print(": ");
  Serial.println(digitalRead(axis.stepPin));
  
  Serial.print("-PIN limit switch ");
  Serial.print(axis.name);
  Serial.print(": ");
  Serial.println(digitalRead(axis.limitSwitchPin));

  Serial.print("-stepsPerUnit ");
  Serial.print(axis.name);
  Serial.print(": ");
  Serial.println(axis.stepsPerUnit);
}

void printAxis(Axis& axis) {
  Serial.print(axis.position);
  Serial.print(",");
  Serial.print(digitalRead(axis.limitSwitchPin));
}
