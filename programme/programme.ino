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
    } else if (input == "v") { // speed
      // TODO: check only if the fist char is a v, then read the rest of the string as a int to get the value of the speed
    } else if (input == "s") { // stop
      setMotorEnabled(false);
    } else if (input == GCODE_HOME_ROUTINE) {
      setMotorEnabled(true);
      isReferencing = true;
    } else if (input == "?") { // debug info
      printDebugInfo();
    } else if (input == "+") {
      setMotorEnabled(true);
      setMotorDirection(CW);
    } else if (input == "-") {
      setMotorEnabled(true);
      setMotorDirection(CCW);
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
  Serial.println("* In *");
  Serial.print("Axis: ");
  Serial.println(selectedAxis);
  Serial.print("Speed: ");
  Serial.println(selectedAxis);
  Serial.println("* Out *");
  Serial.print("Enabled: ");
  Serial.println(digitalRead(motorEnabledPin));
  Serial.print("Dir: ");
  Serial.println(digitalRead(dirPin));
}
