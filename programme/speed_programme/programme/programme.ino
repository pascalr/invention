#include "axis.h"

// CONSTANTS
#define SLOW_SPEED_DELAY 2000
#define FAST_SPEED_DELAY 100

using namespace std;

// FIXME: Il faudrait que j'utilse char array et non String, parce que String c'est seulement sur Arduino et je veux pouvoir debugger...

class ArduinoWriter : public Writer {
  public:
    void doPinMode(int pin, bool type) {
      pinMode(pin, type);
    }
    void doDigitalWrite(int pin, bool value) {
      digitalWrite(pin, value);
    }
    double doDigitalRead(int pin) {
      return digitalRead(pin);
    }
    void doPrint(const char* theString) {
      Serial.print(theString);
    }
    void doPrintLn(const char* theString) {
      Serial.println(theString);
    }
};

Writer* writer;
Axis* axisT;
VerticalAxis* axisY;
HorizontalAxis* axisX;

Axis* axisByLetter(char letter) {
  if (letter == 'X' || letter == 'x') {
    return axisX;
  } else if (letter == 'Y' || letter == 'y') {
    return axisY;
  } else if (letter == 'Z' || letter == 'z') {
    return axisT;
  }
  return NULL;
}

void setup() {

  //Initiate Serial communication.
  Serial.begin(9600);
  Serial.println("Setup...");

  // ************* PIN LAYOUT **************
  writer = new ArduinoWriter();

  // FIXME: Do you need to delete?
  // The setup function is only ran once
  axisX = new HorizontalAxis(writer, 'X',500);
  axisY = new VerticalAxis(writer, 'Y',500);
  axisT = new Axis(writer, 'Z', 500);

  axisX->setRotationAxis(axisT);
  
  axisT->enabledPin = 8;
  axisT->dirPin = 10;
  axisT->stepPin = 11;
  axisT->limitSwitchPin = 12;

  axisX->enabledPin = 8;
  axisX->dirPin = 2;
  axisX->stepPin = 3;
  axisX->limitSwitchPin = 12;

  axisY->enabledPin = 8;
  axisY->dirPin = 7;
  axisY->stepPin = 6;
  axisY->limitSwitchPin = 12;
  // ***************************************
  
  axisX->setupPins();
  axisY->setupPins();
  axisT->setupPins();

  // Linear axes units are mm. Rotary axes units are degrees.
  // Number of steps per turn of the motor * microstepping / distance per turn
  // The value is multiplied by two because we have to write LOW then HIGH for one step
  axisX->stepsPerUnit = 200 * 2 * 8 / (1.25*25.4*3.1416);
  axisY->stepsPerUnit = 200 * 2 * 8 / (2.625*25.4*3.1416);
  axisT->stepsPerUnit = 200 * 2 * 8 / (360*12/61);
  
  axisX->setMotorEnabled(false);
  axisY->setMotorEnabled(false);
  axisT->setMotorEnabled(false);
  
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
    Axis* axis = axisByLetter(cmd[i]);
    if (axis) {
      axis->speed = cmd.substring(i+1,i+1+nbLength).toInt();
    }
    i = i+nbLength;
  }
}

void parseMove(String cmd) {
  for (int i = 0; i < cmd.length(); i++) {
    int nbLength = numberLength(cmd.substring(i+1));
    if (cmd[i] == 'Z' || cmd[i] == 'z') {
      int destination = cmd.substring(i+1,i+1+nbLength).toInt();
      if (destination > 0 && destination <= RAYON) {
        double angle = asin(destination / RAYON) * 180.0 / PI;
        axisT->setDestinationUnit(angle);
      }
    } else {
      Axis* axis = axisByLetter(cmd[i]);
      if (axis) {
        axis->setDestinationUnit(cmd.substring(i+1,i+1+nbLength).toInt());
      }
    }
    i = i+nbLength;
  }
}

void loop() {
  unsigned long currentTime = micros();

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
      axisX->stop();
      axisY->stop();
      axisT->stop();
    } else if (input.charAt(0) == 'H' || input.charAt(0) == 'h') { // home reference (eg. H, or HX, or HY, ...)
      Serial.println("Referencing...");
      if (input.length() == 1) {
        axisX->startReferencing();
        axisY->startReferencing();
        axisT->startReferencing();
      } else {
        Axis* axis = axisByLetter(input.charAt(1));
        if (axis) {
          axis->startReferencing();
        }
      }
    } else if (input == "?") { // debug info
      printDebugInfo();
    } else if (input.charAt(0) == '+') {
      Axis* axis = axisByLetter(input.charAt(1));
      if (axis) {
        axis->rotate(CW);
      }
    } else if (input.charAt(0) == '-') {
      Axis* axis = axisByLetter(input.charAt(1));
      if (axis) {
        axis->rotate(CCW);
      }
    }
  }

  axisX->handleAxis(currentTime);
  axisY->handleAxis(currentTime);
  axisT->handleAxis(currentTime);
}

void printDebugInfo() {
  printDebugAxis(axisX);
  printDebugAxis(axisY);
  printDebugAxis(axisT);
}

void printDebugAxis(Axis* axis) {
  Serial.print("-Pos ");
  Serial.print(axis->name);
  Serial.print(": ");
  Serial.println(axis->position);
  
  Serial.print("-Dest ");
  Serial.print(axis->name);
  Serial.print(": ");
  Serial.println(axis->destination);
  
  Serial.print("-Speed ");
  Serial.print(axis->name);
  Serial.print(": ");
  Serial.println(axis->speed);

  Serial.print("-CW ");
  Serial.print(axis->name);
  Serial.print(": ");
  Serial.println(axis->isClockwise);
  
  Serial.print("-Referenced ");
  Serial.print(axis->name);
  Serial.print(": ");
  Serial.println(axis->isReferenced);
  
  Serial.print("-Referencing ");
  Serial.print(axis->name);
  Serial.print(": ");
  Serial.println(axis->isReferencing);
  
  Serial.print("-Enabled ");
  Serial.print(axis->name);
  Serial.print(": ");
  Serial.println(axis->isMotorEnabled);
  
  Serial.print("-Step ");
  Serial.print(axis->name);
  Serial.print(": ");
  Serial.println(axis->isStepHigh);

  Serial.print("-Force ");
  Serial.print(axis->name);
  Serial.print(": ");
  Serial.println(axis->forceRotation);

  Serial.print("-PIN enabled ");
  Serial.print(axis->name);
  Serial.print(": ");
  Serial.println(digitalRead(axis->enabledPin));
  
  Serial.print("-PIN dir ");
  Serial.print(axis->name);
  Serial.print(": ");
  Serial.println(digitalRead(axis->dirPin));
  
  Serial.print("-PIN step ");
  Serial.print(axis->name);
  Serial.print(": ");
  Serial.println(digitalRead(axis->stepPin));
  
  Serial.print("-PIN limit switch ");
  Serial.print(axis->name);
  Serial.print(": ");
  Serial.println(digitalRead(axis->limitSwitchPin));

  Serial.print("-stepsPerUnit ");
  Serial.print(axis->name);
  Serial.print(": ");
  Serial.println(axis->stepsPerUnit);
}

void printAxis(Axis* axis) {
  Serial.print(axis->position);
  Serial.print(",");
  Serial.print(digitalRead(axis->limitSwitchPin));
}
