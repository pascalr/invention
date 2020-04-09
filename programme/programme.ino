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
#define SLOW_SPEED_DELAY 10000
#define FAST_SPEED_DELAY 100

#define CW true
#define CCW false

struct axis
{
   unsigned long position;
   unsigned long destination;
   unsigned int speed;
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
   char name;
};

typedef struct axis Axis;

Axis axisX, axisY, axisZ, axisW, oups;

unsigned long currentTime;

// setters for output pins
void setMotorsEnabled(bool value) {
  digitalWrite(axisY.enabledPin, LOW); // FIXME: ALWAYS ENABLED
  //digitalWrite(axisY.enabledPin, value ? LOW : HIGH);
  axisY.isMotorEnabled = value;
  
  digitalWrite(ledPin, value ? HIGH : LOW);
}

void setMotorsDirection(bool clockwise) {
  digitalWrite(axisY.dirPin, clockwise ? LOW : HIGH);
  axisY.isClockwise = clockwise;
  
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
  
  axisY.enabledPin = 8;
  axisY.dirPin = 2;
  axisY.stepPin = 3;
  axisY.limitSwitchPin = 12;
  // ***************************************
  
  setupAxis(axisY, 'Y', 500);
  
  setMotorsEnabled(false);
  setMotorsDirection(CW);
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
    axis.destination = cmd.substring(i+1,i+1+nbLength).toInt();
    setMotorsDirection(axis.destination > axis.position);
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
      setMotorsEnabled(false);
      axis.isReferenced = true;
      axis.isReferencing = false;
    } else {
      turnOneStep(axis);
      delayMicroseconds(SLOW_SPEED_DELAY);
    }
  } else if (axis.isReferenced && axis.isMotorEnabled && currentTime - axis.previousStepTime > axis.speed &&
            ((axis.isClockwise && axis.position < axis.destination) || (!axis.isClockwise && axis.position > axis.destination))) {
    turnOneStep(axis);
    axis.previousStepTime = currentTime;
  }
}

Axis& axisByLetter(char letter) {
  if (letter == 'X' || letter == 'x') {
    return axisX;
  } else if (letter == 'Y' || letter == 'y') {
    return axisY;
  } else if (letter == 'Z' || letter == 'z') {
    return axisZ;
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
      setMotorsEnabled(true);
      parseMove(input.substring(1));
    } else if (input.charAt(0) == 'V' || input.charAt(0) == 'v') { // speed (eg. VX300 -> axis X speed 300 microseconds delay per step)
      parseSpeed(input.substring(1));
    } else if (input.charAt(0) == 's' || input.charAt(0) == 'S') { // stop
      //setMotorsEnabled(false);
      axisX.destination = axisX.position;
      axisY.destination = axisY.position;
      axisZ.destination = axisZ.position;
      axisW.destination = axisW.position;
    } else if (input.charAt(0) == 'H' || input.charAt(0) == 'h') { // home reference (eg. H, or HX, or HY, ...)
      Serial.println("Referencing...");
      setMotorsEnabled(true);
      if (input.length() == 1) {
        axisX.isReferencing = true;
        axisY.isReferencing = true;
        axisZ.isReferencing = true;
        axisW.isReferencing = true;
      } else {
        axisByLetter(input.charAt(1)).isReferencing = true;
      }
    } else if (input == "?") { // debug info
      printDebugInfo();
    } else if (input == "+") {
      setMotorsDirection(CW);
      setMotorsEnabled(true);
    } else if (input == "-") {
      setMotorsDirection(CCW);
      setMotorsEnabled(true);
    }
  }

  handleAxis(axisY);
}

void printDebugInfo() {
  Serial.println("***");
  printDebugAxis(axisY);
}

void printDebugAxis(Axis& axis) {
  Serial.print("Axis ");
  Serial.println(axis.name);
  
  Serial.print("Pos: ");
  Serial.println(axis.position);
  Serial.print("Dest: ");
  Serial.println(axis.destination);
  Serial.print("Speed: ");
  Serial.println(axis.speed);

  Serial.print("CW: ");
  Serial.println(axis.isClockwise);
  Serial.print("Referenced: ");
  Serial.println(axis.isReferenced);
  Serial.print("Referencing: ");
  Serial.println(axis.isReferencing);
  Serial.print("Enabled: ");
  Serial.println(axis.isMotorEnabled);
  Serial.print("Step: ");
  Serial.println(axis.isStepHigh);

  Serial.print("PIN enabled: ");
  Serial.println(digitalRead(axis.enabledPin));
  Serial.print("PIN dir: ");
  Serial.println(digitalRead(axis.dirPin));
  Serial.print("PIN step: ");
  Serial.println(digitalRead(axis.stepPin));
  Serial.print("PIN limit switch: ");
  Serial.println(digitalRead(axis.limitSwitchPin));
}

void printAxis(Axis& axis) {
  Serial.print(axis.position);
  Serial.print(",");
  Serial.print(digitalRead(axis.limitSwitchPin));
}
