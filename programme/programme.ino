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
#define Y_MOTOR_ENABLED_PIN 8
#define Y_DIR_PIN 2
#define Y_STEP_PIN 3
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

struct axis
{
   int position;
   int speed;
   int enabledPin;
   int dirPin;
   int stepPin;
};

typedef struct axis Axis;

Axis axisX, axisY, axisZ, axisW;

unsigned long previousTime = micros();

unsigned long positionX = 0;
unsigned long positionY = 0;
unsigned long positionZ = 0;
unsigned long positionW = 0;

bool motorStep = false;

bool isMotorEnabled = false;
bool isClockwise = true;
bool isManualMode = true;

bool isReferenced = false;
bool isReferencing = false;

int selectedAxis = 0; // Only one axis is selected at a time (X, Y, Z or W) in manual mode

int selectedSpeed = 300;

int destX = -1; // destination required by the user
int destY = -1; // destination required by the user
int destZ = -1; // destination required by the user
int destW = -1; // destination required by the user

// setters for output pins
void setMotorsEnabled(bool value) {
  digitalWrite(axisY.enabledPin, value ? LOW : HIGH);
  digitalWrite(ledPin, value ? HIGH : LOW);
  isMotorEnabled = value;
}

void setMotorsDirection(bool clockwise) {
  digitalWrite(axisY.dirPin, clockwise ? LOW : HIGH);
  delayMicroseconds(SLOW_SPEED_DELAY);
}

void cookRice() {
  Serial.print("Preparing to cook rice!");
  moveX(RICE_POSITION_X);
  delay(2);
  moveX(RICE_COOKER_POSITION_X);
}

void moveX(int destinationX) {
  setMotorsEnabled(true);
  setMotorsDirection(positionX < destinationX);
  while ((isClockwise && positionX < destinationX) || (!isClockwise && positionX > destinationX)) {
    turnOneStep();
    delayMicroseconds(FAST_SPEED_DELAY);
  }
  setMotorsEnabled(false);
}

void turnOneStep() {
  digitalWrite(Y_STEP_PIN, motorStep ? HIGH : LOW);
  motorStep = !motorStep;
  positionX = isClockwise ? positionX + 1 : positionX - 1;
}

void setupAxis(Axis axis) {
  axis.position = -1;
  
  pinMode(axis.stepPin, OUTPUT);
  pinMode(axis.dirPin, OUTPUT);
  pinMode(axis.enabledPin, OUTPUT);
}

void setup() {

  //Initiate Serial communication.
  Serial.begin(9600);
  Serial.println("Setup...");

  pinMode(ledPin, OUTPUT);

  axisY.speed = 300;
  axisY.enabledPin = 8;
  axisY.dirPin = 2;
  axisY.stepPin = 3;
  
  setupAxis(axisY);
  
  setMotorsEnabled(false);
  setMotorsDirection(CW);
  Serial.println("Done");
}

int parseNextNumber(String str) {
  int i;
  for (i = 0; i < str.length(); i++) {
    if (str[i] < '0' || str[i] > '9') {break;}
  }
  return str.substring(0,i).toInt();
}

void parseMove(String cmd) {
  for (int i = 0; i < cmd.length(); i++) {
    if (cmd[i] == "X") {
      destX = parseNextNumber(cmd.substring(i+1));
    } else if (cmd[i] == "Y") {
      destY = parseNextNumber(cmd.substring(i+1));
    } else if (cmd[i] == "Z") {
      destZ = parseNextNumber(cmd.substring(i+1));
    } else if (cmd[i] == "W") {
      destW = parseNextNumber(cmd.substring(i+1));
    }
  }
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
    } else if (input[0] == "M") {
      parseMove(input.substring(1));
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
      setMotorsEnabled(false);
      destX = -1;
      destY = -1;
      destZ = -1;
      destW = -1;
    } else if (input == GCODE_HOME_ROUTINE) {
      setMotorsEnabled(true);
      isReferencing = true;
    } else if (input == "?") { // debug info
      printDebugInfo();
    } else if (input == "+") {
      setMotorsDirection(CW);
      setMotorsEnabled(true);
    } else if (input == "t") {
      setMotorsDirection(CW);
      setMotorsEnabled(true);
      delayMicroseconds(2000000);
      setMotorsEnabled(false);
    } else if (input == "-") {
      setMotorsDirection(CCW);
      setMotorsEnabled(true);
    } else if (input == "m") { // manual
    }
  }

  if (isReferencing) {
    bool limitSwitchActivated = digitalRead(limitSwitchPin);
    if (limitSwitchActivated) {
      positionX = 0;
      positionY = 0;
      positionZ = 0;
      positionW = 0;
      //moveX(HOME_POSITION_X);
      setMotorsEnabled(false);
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
  Serial.println(digitalRead(Y_MOTOR_ENABLED_PIN));
  Serial.print("Dir: ");
  Serial.println(digitalRead(Y_DIR_PIN));
}
