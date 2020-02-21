// Programme pour ma machine a cuisiner!

#define startPin 4
#define stopPin 7
#define motorEnabledPin 8
#define potPin 2
#define dirPin 2
#define stepPin 3
#define ledPin 13

bool verbose = false;
unsigned long previousTime = micros();
long timeInterval = 0;
long minTimeInterval = 20;
int potVal = 0;
bool motorStep = false;
bool isMotorEnabled = false;
int oldStartButtonState = 0;
int startButtonState = 0;
int oldStopButtonState = 0;
int stopButtonState = 0;

void setup() {

  //Initiate Serial communication.
  Serial.begin(9600);
  
  pinMode(ledPin, OUTPUT);
  pinMode(stepPin, OUTPUT);
  pinMode(dirPin, OUTPUT);
  pinMode(motorEnabledPin, OUTPUT);
  // Set the spinning direction CW/CCW:
  digitalWrite(dirPin, HIGH);
  digitalWrite(motorEnabledPin, HIGH);
}

void loop() {
  unsigned long currentTime = micros();
  potVal = analogRead(potPin);
  if (potVal < minTimeInterval) {
    timeInterval = minTimeInterval;
  } else {
    timeInterval = potVal;
  }
  startButtonState = digitalRead(startPin);
  stopButtonState = digitalRead(stopPin);

  // start button is pressed
  if (oldStartButtonState == LOW && startButtonState == HIGH) {
    isMotorEnabled = true;
    Serial.print("\n*** Start button is pressed\n");
    digitalWrite(motorEnabledPin, LOW);
    digitalWrite(ledPin, HIGH);
  }

  // stop button is pressed
  if (oldStopButtonState == LOW && stopButtonState == HIGH) {
    isMotorEnabled = false;
    Serial.print("\n*** Stop button is pressed\n");
    digitalWrite(motorEnabledPin, HIGH);
    digitalWrite(ledPin, LOW);
  }

  if (isMotorEnabled) {
    if (currentTime - previousTime > timeInterval) {
      if (motorStep) {
        digitalWrite(stepPin, HIGH);
      } else {
        digitalWrite(stepPin, LOW);
      }
      motorStep = !motorStep;
      previousTime = currentTime;
    }
  }

  oldStartButtonState = startButtonState;
  oldStopButtonState = stopButtonState;

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
