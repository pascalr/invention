// Programme pour ma machine a cuisiner!

// INPUT PINS
// buttons
#define startPin 4
#define stopPin 7
#define limitSwitchPin 12
#define referenceButtonPin 13
#define plusPin 10
#define minusPin 9
// potentionmeterss
#define potPin 2
// switches
#define manualSwitchPin 11

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

#define DISPLAY_TIME_INTERVAL 10000000

String input = "";

bool verbose = false;

unsigned long previousTime = micros();
unsigned long previousDisplayTime = micros();

long timeInterval = 0;
long minTimeInterval = 20;
int potVal = 0;
bool motorStep = false;
bool isMotorEnabled = false;

int oldStartButtonState = 0;
int oldStopButtonState = 0;
int oldReferenceButtonState = 0;

unsigned long positionX = 0;
bool isReferenced = false;
bool isClockwise = true;

bool eStop() {
  bool val = digitalRead(stopPin) == HIGH;
  if (val) {
    setMotorEnabled(false);
  }
  return val;
}

// setters for output pins
void setMotorEnabled(bool value) {
  Serial.print("\n*** Motor enabled is set to:");
  Serial.print(value);
  Serial.print("\n");
  digitalWrite(motorEnabledPin, value ? LOW : HIGH);
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
    if (eStop) {return}
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

void doReference() {
  Serial.print("Doing referencing...");
  setMotorEnabled(true);
  while (true) {  // while noStop()
    if (eStop) {return}
    bool limitSwitchActivated = digitalRead(limitSwitchPin);
    if (limitSwitchActivated) {
      positionX = 0;
      moveX(HOME_POSITION_X);
      isReferenced = true;
      return;
    } else {  // Turn motor at slow speed
      turnOneStep();
      delayMicroseconds(SLOW_SPEED_DELAY);
    }
  }
}

void setup() {

  //Initiate Serial communication.
  Serial.begin(9600);
  
  pinMode(ledPin, OUTPUT);
  pinMode(stepPin, OUTPUT);
  pinMode(dirPin, OUTPUT);
  pinMode(motorEnabledPin, OUTPUT);
  // Set the spinning direction CW/CCW:
  digitalWrite(dirPin, HIGH);
  setMotorEnabled(false);
 // digitalWrite(motorEnabledPin, HIGH);
}

void loop() {
  //Serial.print(digitalRead(referenceButtonPin));
  //Serial.print('\n');
  unsigned long currentTime = micros();
  potVal = analogRead(potPin);
  if (potVal < minTimeInterval) {
    timeInterval = minTimeInterval;
  } else {
    timeInterval = potVal;
  }

  if (Serial.available() > 0) {
    // read the incoming byte:
    input = Serial.readString();

    Serial.print("I received: ");
    Serial.println(input);
    if (input == "riz\n") {
      cookRice();
    } else if (input == "stop\n") {
      setMotorEnabled(false);
    }
  }

  int startButtonState = digitalRead(startPin);
  int stopButtonState = digitalRead(stopPin);
  int referenceButtonState = digitalRead(referenceButtonPin);

  // start button is pressed
  if (isReferenced && oldStartButtonState == HIGH && startButtonState == LOW) {
    setMotorEnabled(true);
  }

  // stop button is pressed
  if (oldStopButtonState == HIGH && stopButtonState == LOW) {
    setMotorEnabled(false);
  }

  // reference button is pressed
  if (oldReferenceButtonState == HIGH && referenceButtonState == LOW) {
    doReference();
  }

  if (isMotorEnabled) {
    if (currentTime - previousTime > timeInterval) {
      turnOneStep();
      previousTime = currentTime;
    }
  }

  oldStartButtonState = startButtonState;
  oldStopButtonState = stopButtonState;
  oldReferenceButtonState = referenceButtonState;

  // Display debug info
  if (verbose || currentTime - previousDisplayTime > DISPLAY_TIME_INTERVAL) {
    Serial.print("\n* ");
    Serial.print((currentTime - previousTime)/1000000);
    Serial.print(" s *\n");
    Serial.print("start\n");
    Serial.print(startButtonState);
    Serial.print("\nstop\n");
    Serial.print(stopButtonState);
    Serial.print("\npot\n");
    Serial.print(potVal);
    Serial.print("\nstep\n");
    Serial.print(motorStep);
    Serial.print("\nen\n");
    Serial.print(isMotorEnabled);
    Serial.print("\nX\n");
    Serial.print(positionX);
    previousDisplayTime = currentTime;
  }
}
