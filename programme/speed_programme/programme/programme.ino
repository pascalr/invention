#include <GSM3VoiceCallService.h>
#include <GSM3ShieldV1ModemCore.h>
#include <GSM3MobileClientService.h>
#include <GSM3CircularBuffer.h>
#include <GSM3MobileSMSProvider.h>
#include <GSM3ShieldV1DataNetworkProvider.h>
#include <GSM3MobileDataNetworkProvider.h>
#include <GSM3MobileMockupProvider.h>
#include <GSM3MobileAccessProvider.h>
#include <GSM3MobileServerService.h>
#include <GSM3ShieldV1BaseProvider.h>
#include <GSM3ShieldV1CellManagement.h>
#include <GSM3ShieldV1ModemVerification.h>
#include <GSM3ShieldV1MultiServerProvider.h>
#include <GSM3MobileNetworkRegistry.h>
#include <GSM3ShieldV1SMSProvider.h>
#include <GSM3ShieldV1BandManagement.h>
#include <GSM3ShieldV1.h>
#include <GSM3ShieldV1ServerProvider.h>
#include <GSM3ShieldV1VoiceProvider.h>
#include <GSM3SMSService.h>
#include <GSM3MobileServerProvider.h>
#include <GSM.h>
#include <GSM3ShieldV1ScanNetworks.h>
#include <GSM3ShieldV1ClientProvider.h>
#include <GSM3MobileClientProvider.h>
#include <GSM3MobileCellManagement.h>
#include <GSM3MobileNetworkProvider.h>
#include <GSM3ShieldV1MultiClientProvider.h>
#include <GSM3SoftSerial.h>
#include <GSM3ShieldV1DirectModemProvider.h>
#include <GSM3IO.h>
#include <GSM3MobileVoiceProvider.h>
#include <GSM3ShieldV1PinManagement.h>
#include <GSM3ShieldV1AccessProvider.h>

// OUTPUT PINS
#define ledPin 13

// CONSTANTS
#define SLOW_SPEED_DELAY 2000
#define FAST_SPEED_DELAY 100

#define CW true
#define CCW false

// Linear axes units are mm. Rotary axes units are degrees.
#define CALC_STEPS_PER_UNIT(MOTOR_STEPS_PER_TURN, MICROSTEPPING, DISTANCE_PER_TURN)  (MOTOR_STEPS_PER_TURN * MICROSTEPPING / DISTANCE_PER_TURN)

class Axis {
  public:
    unsigned long position;
    unsigned long destination;
    unsigned long maxPosition;
    unsigned int speed;
    int enabledPin;
    int dirPin;
    int stepPin;
    int limitSwitchPin;
    unsigned long previousStepTime;
    double stepsPerUnit;
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
  private:
  Axis& m_rotary_axis;
  
  public:
  
  unsigned int getDelay() {
    // TODO: The speed is not fixed.
    return speed;
  }
};

Axis axisY, axisT, axisW, oups;
HorizontalAxis axisX;

unsigned long currentTime;

void setMotorEnabled(Axis& axis, bool value) {
  digitalWrite(axis.enabledPin, LOW); // FIXME: ALWAYS ENABLED
  //digitalWrite(axisY.enabledPin, value ? LOW : HIGH);
  axis.isMotorEnabled = value;
  
  digitalWrite(ledPin, value ? HIGH : LOW);
}

void setMotorDirection(Axis& axis, bool clockwise) {
  digitalWrite(axis.dirPin, clockwise ? LOW : HIGH);
  axis.isClockwise = clockwise;
  
  delayMicroseconds(SLOW_SPEED_DELAY);
}

void turnOneStep(Axis& axis) {
  digitalWrite(axis.stepPin, axis.isStepHigh ? LOW : HIGH);
  axis.isStepHigh = !axis.isStepHigh;
  axis.position = axis.position + (axis.isClockwise ? 1 : -1);
}

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

  axisX.stepsPerUnit = CALC_STEPS_PER_UNIT(200, 8, 2.625*25.4*3.1416);
  axisY.stepsPerUnit = CALC_STEPS_PER_UNIT(200, 8, 1.25*25.4*3.1416);
  axisT.stepsPerUnit = CALC_STEPS_PER_UNIT(200, 8, 360*61/12);
  
  setMotorEnabled(axisX,false);
  setMotorEnabled(axisY,false);
  setMotorEnabled(axisT,false);
  
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
    setMotorEnabled(axis, true);
    setMotorDirection(axis, axis.destination > axis.position);
    i = i+nbLength;
  }
}

void handleAxis(Axis& axis) {
  if (axis.isReferencing) {
    //Serial.println(digitalRead(axis.limitSwitchPin));
    if (!digitalRead(axis.limitSwitchPin)) {
      Serial.print("Done referencing axis ");
      Serial.println(axis.name);
      axis.position = 0;
      setMotorEnabled(axis, false);
      axis.isReferenced = true;
      axis.isReferencing = false;
    } else {
      turnOneStep(axis);
      delayMicroseconds(SLOW_SPEED_DELAY);
    }
  } else if (axis.isReferenced && axis.isMotorEnabled && currentTime - axis.previousStepTime > axis.speed && (axis.forceRotation ||
            ((axis.isClockwise && axis.position < axis.destination) || (!axis.isClockwise && axis.position > axis.destination)))) {
    turnOneStep(axis);
    if (currentTime - axis.previousStepTime > 2*axis.speed) {
      axis.previousStepTime = currentTime; // refreshing previousStepTime when it is the first step and the motor was at a stop
    } else {
      axis.previousStepTime = axis.previousStepTime + axis.speed; // This is more accurate to ensure all the motors are synchronysed
    }
  }
}

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
        setMotorDirection(axisX, CCW);
        setMotorDirection(axisY, CCW);
        setMotorDirection(axisT, CCW);
        setMotorEnabled(axisX, true);
        setMotorEnabled(axisY, true);
        setMotorEnabled(axisT, true);
      } else {
        Axis& axis = axisByLetter(input.charAt(1));
        axis.isReferencing = true;
        setMotorDirection(axis, CCW);
        setMotorEnabled(axis, true);
      }
    } else if (input == "?") { // debug info
      printDebugInfo();
    } else if (input.charAt(0) == '+') {
      Axis& axis = axisByLetter(input.charAt(1));
      setMotorDirection(axis,CW);
      axis.forceRotation = true;
      setMotorEnabled(axis,true);
    } else if (input.charAt(0) == '-') {
      Axis& axis = axisByLetter(input.charAt(1));
      setMotorDirection(axis,CCW);
      axis.forceRotation = true;
      setMotorEnabled(axis, true);
    }
  }

  handleAxis(axisX);
  handleAxis(axisY);
  handleAxis(axisT);
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
}

void printAxis(Axis& axis) {
  Serial.print(axis.position);
  Serial.print(",");
  Serial.print(digitalRead(axis.limitSwitchPin));
}
