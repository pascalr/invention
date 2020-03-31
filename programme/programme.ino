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
#define SLOW_SPEED_DELAY 500
#define FAST_SPEED_DELAY 100

#define CW true
#define CCW false

struct axis
{
   int position;
   int destination;
   int speed;
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
};

typedef struct axis Axis;

Axis axisX, axisY, axisZ, axisW, oups;

unsigned long currentTime;

// setters for output pins
void setMotorsEnabled(bool value) {
  digitalWrite(axisY.enabledPin, LOW); // FIXME: ALWAYS ENABLED
  // FIXME: digitalWrite(axisY.enabledPin, value ? LOW : HIGH);
  axisY.isMotorEnabled = value;
  
  digitalWrite(ledPin, value ? HIGH : LOW);
}

void setMotorsDirection(bool clockwise) {
  digitalWrite(axisY.dirPin, clockwise ? LOW : HIGH);
  axisY.isClockwise = clockwise;
  
  delayMicroseconds(SLOW_SPEED_DELAY);
}

void turnOneStep(Axis axis) {
  digitalWrite(axis.stepPin, axis.isStepHigh ? LOW : HIGH);
  axis.isStepHigh = !axis.isStepHigh;
  axis.position = axis.position + (axis.isClockwise ? 1 : -1);
}

void setupAxis(Axis axis, int speed) {
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
  
  setupAxis(axisY, 300);
  
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

void parseMove(String cmd) {
  for (int i = 0; i < cmd.length(); i++) {
    int nbLength = numberLength(cmd.substring(i+1));
    axisByLetter(cmd[i]).destination = cmd.substring(i+1,i+1+nbLength).toInt();
    i = i+nbLength;
  }
}

void handleAxis(Axis axis) {
  if (axis.isReferencing) {
    if (digitalRead(axis.limitSwitchPin)) {
      axis.position = 0;
      setMotorsEnabled(false);
      axis.isReferenced = true;
      axis.isReferencing = false;
    } else {
      turnOneStep(axis);
      delayMicroseconds(SLOW_SPEED_DELAY);
    }
  } else if (axis.isReferenced && axis.isMotorEnabled && currentTime - axis.previousStepTime > axis.speed &&
            (axis.isClockwise && axis.position < axis.destination) || (!axis.isClockwise && axis.position > axis.destination)) {
    turnOneStep(axis);
    axis.previousStepTime = currentTime;
  }
}

Axis axisByLetter(char letter) {
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
    input[input.length()-1] = '\0'; // remove trailing newline char

    Serial.print("Cmd: ");
    Serial.println(input);
    if (input[0] == "M") {
      setMotorsEnabled(true);
      parseMove(input.substring(1));
    } else if (input.charAt(0) == 'V') { // speed (eg. VX300 -> axis X speed 300 microseconds delay per step)
      axisByLetter(input.charAt(1)).speed = input.substring(2).toInt();
    } else if (input == "s") { // stop
      setMotorsEnabled(false);
    } else if (input == "H") { // home reference (eg. H, or HX, or HY, ...)
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
  printDebugAxis(axisY, "Y");
}

void printDebugAxis(Axis axis, String name) {
  Serial.print("Axis ");
  Serial.println(name);
  
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

void printAxis(Axis axis) {
  Serial.print(axis.position);
  Serial.print(",");
  Serial.print(digitalRead(axis.limitSwitchPin));
}
