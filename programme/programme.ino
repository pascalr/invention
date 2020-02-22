// Programme pour ma machine a cuisiner!

// INPUT PINS
#define startPin 4
#define stopPin 7
#define potPin 2
#define limitSwitchPin 12
#define referenceButtonPin 13

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
#define RICE_COOKER_POSITION_X 100000

bool verbose = false;
unsigned long previousTime = micros();
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

// setters for output pins
void setMotorEnabled(bool value) {
  Serial.print("\n*** Motor enabled is set to:");
  Serial.print(value);
  Serial.print("\n");
  digitalWrite(motorEnabledPin, value ? LOW : HIGH);
  isMotorEnabled = value;
}

void cookRice() {
  moveX(RICE_POSITION_X);
  delay(2);
  moveX(RICE_COOKER_POSITION_X);
}

void moveX(int destinationX) {
  bool isClockwise = positionX < destinationX; // clockwise?
  digitalWrite(dirPin, isClockwise ? LOW : HIGH);
  delayMicroseconds(SLOW_SPEED_DELAY);
  while ((isClockwise && positionX < destinationX) || (!isClockwise && positionX > destinationX)) {
    turnOneStep();
    delayMicroseconds(FAST_SPEED_DELAY);
  }
}

void turnOneStep() {
  digitalWrite(stepPin, motorStep ? HIGH : LOW);
  motorStep = !motorStep;
  positionX = isClockwise ? positionX + 1 : positionX - 1;
}

void doReference() {
  Serial.print("Doing referencing...");
  setMotorEnabled(true);
  while (true) {
    bool limitSwitchActivated = digitalRead(limitSwitchPin);
    if (limitSwitchActivated) {
      positionX = 0;
      moveX(HOME_POSITION_X);
      setMotorEnabled(false);
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
  if (verbose) {
    Serial.print("\n----------------------- Loop ");
    Serial.print(currentTime - previousTime);
    Serial.print(" --------------------------\n");
    Serial.print("startButtonState\n");
    Serial.print(startButtonState);
    Serial.print("\nstopButtonState\n");
    Serial.print(stopButtonState);
    Serial.print("\npotVal\n");
    Serial.print(potVal);
    Serial.print("\nmotorStep\n");
    Serial.print(motorStep);
    Serial.print("\nisMotorEnabled\n");
    Serial.print(isMotorEnabled);
    Serial.print("\n\n\n\n\n\n");
  }
}
