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

// Programme pour ma machine a cuisiner!
// Version 2, controler par mon application web!

// gcode tutorial
// https://www.simplify3d.com/support/articles/3d-printing-gcode-tutorial/

#define GCODE_HOME_ROUTINE "G28"
// GCODE G28 home routine
// G28 X Y ; home X and Y axes
// G28 Z ; home Z axis only

// INPUT PINS
#define limitSwitchPin 12

// OUTPUT PINS
#define motorEnabledPin 8
#define dirPin 2
#define stepPin 3
#define ledPin 13

// CONSTANTS
#define SLOW_SPEED_DELAY 500
#define FAST_SPEED_DELAY 100
#define HOME_POSITION_X 20000
#define RICE_POSITION_X 50000
#define RICE_COOKER_POSITION_X 50000

#define CW true
#define CCW false

#define AXIS_X 1
#define AXIS_Y 2
#define AXIS_Z 3

unsigned long previousTime = micros();

unsigned long positionX = 0;

bool motorStep = false;

bool isMotorEnabled = false;
bool isClockwise = true;
bool isManualMode = true;

bool isReferenced = false;
bool isReferencing = false;

int selectedAxis = 0; // Only one axis is selected at a time (X, Y, Z or W) in manual mode

int selectedSpeed = 300;

// setters for output pins
void setMotorEnabled(bool value) {
  digitalWrite(motorEnabledPin, value ? LOW : HIGH);
  digitalWrite(ledPin, value ? HIGH : LOW);
  isMotorEnabled = value;
}

void setMotorDirection(bool clockwise) {
  digitalWrite(dirPin, clockwise ? LOW : HIGH);
  delayMicroseconds(SLOW_SPEED_DELAY);
}

void cookRice() {
  Serial.print("Preparing to cook rice!");
  moveX(RICE_POSITION_X);
  delay(2);
  moveX(RICE_COOKER_POSITION_X);
}

void moveX(int destinationX) {
  setMotorEnabled(true);
  setMotorDirection(positionX < destinationX);
  while ((isClockwise && positionX < destinationX) || (!isClockwise && positionX > destinationX)) {
    turnOneStep();
    delayMicroseconds(FAST_SPEED_DELAY);
  }
  setMotorEnabled(false);
}

void turnOneStep() {
  digitalWrite(stepPin, motorStep ? HIGH : LOW);
  motorStep = !motorStep;
  positionX = isClockwise ? positionX + 1 : positionX - 1;
}

void setup() {

  //Initiate Serial communication.
  Serial.begin(9600);
  Serial.println("Setup...");
  
  pinMode(ledPin, OUTPUT);
  pinMode(stepPin, OUTPUT);
  pinMode(dirPin, OUTPUT);
  pinMode(motorEnabledPin, OUTPUT);
  
  setMotorEnabled(false);
  setMotorDirection(CW);
  Serial.println("Done");
}

void loop() {
  unsigned long currentTime = micros();

  if (Serial.available() > 0) {
    String input = Serial.readString();
    input[input.length()-1] = '\0'; // remove trailing newline char

    Serial.print("Cmd: ");
    Serial.println(input);
    if (input == "riz\n") {
      cookRice();
    } else if (input == "x") {
      selectedAxis = selectedAxis == AXIS_X ? 0 : AXIS_X;
    } else if (input == "y") {
      selectedAxis = selectedAxis == AXIS_Y ? 0 : AXIS_Y;
    } else if (input == "z") {
      selectedAxis = selectedAxis == AXIS_Z ? 0 : AXIS_Z;
    } else if (input.charAt(0) == 'v') { // speed
      Serial.println(input.substring(1));
      Serial.println(input.substring(1).toInt());
      selectedSpeed = input.substring(1).toInt();
    } else if (input == "s") { // stop
      setMotorEnabled(false);
    } else if (input == GCODE_HOME_ROUTINE) {
      setMotorEnabled(true);
      isReferencing = true;
    } else if (input == "?") { // debug info
      printDebugInfo();
    } else if (input == "+") {
      setMotorDirection(CW);
      setMotorEnabled(true);
    } else if (input == "t") {
      setMotorDirection(CW);
      setMotorEnabled(true);
      delayMicroseconds(2000000);
      setMotorEnabled(false);
    } else if (input == "-") {
      setMotorDirection(CCW);
      setMotorEnabled(true);
    } else if (input == "m") { // manual
    }
  }

  if (isReferencing) {
    bool limitSwitchActivated = digitalRead(limitSwitchPin);
    if (limitSwitchActivated) {
      positionX = 0;
      //moveX(HOME_POSITION_X);
      setMotorEnabled(false);
      isReferenced = true;
      isReferencing = false;
    } else {
      turnOneStep();
      delayMicroseconds(SLOW_SPEED_DELAY);
    }
  } else if (isReferenced && isMotorEnabled && currentTime - previousTime > selectedSpeed) {
    turnOneStep();
    previousTime = currentTime;
  }
}

void printDebugInfo() {
  Serial.println("***");
  Serial.print("Axis: ");
  Serial.println(selectedAxis);
  Serial.print("Speed: ");
  Serial.println(selectedSpeed);
  Serial.print("PositionX: ");
  Serial.println(positionX);
  Serial.print("Is manual mode: ");
  Serial.println(isManualMode);
  Serial.print("isReferenced: ");
  Serial.println(isReferenced);
  Serial.print("isReferencing: ");
  Serial.println(isReferencing);
  Serial.println("* In *");
  Serial.print("Limit switch pin: ");
  Serial.println(digitalRead(limitSwitchPin));
  Serial.println("* Out *");
  Serial.print("Enabled: ");
  Serial.println(digitalRead(motorEnabledPin));
  Serial.print("Dir: ");
  Serial.println(digitalRead(dirPin));
}
