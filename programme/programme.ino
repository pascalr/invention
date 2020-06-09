#include "axis.h"
#include "setup.h"
#include "common.h" 

// CONSTANTS
#define SLOW_SPEED_DELAY 2000
#define FAST_SPEED_DELAY 100

#define MAX_INPUT_CHUNK_SIZE 256

using namespace std;

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
Axis* axisA;
Axis* axisB;

Axis* axes[10];

void setup() {

  //Initiate Serial communication.
  Serial.begin(9600);
  //Serial.begin(115200);
  Serial.println("Setup...");

  // FIXME: Do you need to delete?
  // The setup function is only ran once
  writer = new ArduinoWriter();
  axisX = new HorizontalAxis(writer, 'X');
  axisY = new VerticalAxis(writer, 'Y');
  axisT = new Axis(writer, 'T');
  axisA = new Axis(writer, 'A');
  axisB = new Axis(writer, 'B');
  axes[0] = axisX;
  axes[1] = axisY;
  axes[2] = axisT;
  axes[3] = axisA;
  axes[4] = axisB;
  axes[5] = NULL;
  setupAxes(writer, axes);
  
  Serial.println("Done");
}

void loop() {
  unsigned long currentTime = micros();

  if (Serial.available() > 0) {
    /*String input = Serial.readString();
    char msg[256] = {0};
    input.toCharArray(msg, 256);
    input.remove(input.length()-1);*/

    // Get next command from Serial (add 1 for final 0)
    char input[MAX_INPUT_CHUNK_SIZE + 1];
    byte size = Serial.readBytes(input, MAX_INPUT_CHUNK_SIZE);
    input[size] = 0; // Add the final 0 to end the C string

    Serial.print("Cmd: ");
    Serial.println(input);
    if (input.charAt(0) == 'M' || input.charAt(0) == 'm') {
      parseMove(axes, msg+1);
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
        Axis* axis = axisByLetter(axes, input.charAt(1));
        if (axis) {
          axis->startReferencing();
        }
      }
    } else if (input == "?") { // debug info
      printDebugInfo();
    } else if (input.charAt(0) == '+') {
      Axis* axis = axisByLetter(axes, input.charAt(1));
      if (axis) {
        axis->rotate(CW);
      }
    } else if (input.charAt(0) == '-') {
      Axis* axis = axisByLetter(axes, input.charAt(1));
      if (axis) {
        axis->rotate(CCW);
      }
    }
  }

  axisX->handleAxis(currentTime);
  axisY->handleAxis(currentTime);
  axisT->handleAxis(currentTime);
}

/* https://arduino.stackexchange.com/questions/1013/how-do-i-split-an-incoming-string
 // Calculate based on max input size expected for one command
#define INPUT_SIZE 30
...

// Get next command from Serial (add 1 for final 0)
char input[INPUT_SIZE + 1];
byte size = Serial.readBytes(input, INPUT_SIZE);
// Add the final 0 to end the C string
input[size] = 0;

// Read each command pair 
char* command = strtok(input, "&");
while (command != 0)
{
    // Split the command in two values
    char* separator = strchr(command, ':');
    if (separator != 0)
    {
        // Actually split the string in 2: replace ':' with 0
        *separator = 0;
        int servoId = atoi(command);
        ++separator;
        int position = atoi(separator);

        // Do something with servoId and position
    }
    // Find the next command in input string
    command = strtok(0, "&");
}*/

void printDebugInfo() {
  printDebugAxis(axisX);
  printDebugAxis(axisY);
  printDebugAxis(axisT);
}

void printDebugAxis(Axis* axis) {
  // TODO: Turn the axis into a char[], then print the char[], so I can debug too.
  Serial.print("-Pos ");
  Serial.print(axis->name);
  Serial.print(": ");
  Serial.println(axis->getPosition());
  
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
  Serial.print(axis->getPosition());
  Serial.print(",");
  Serial.print(digitalRead(axis->limitSwitchPin));
}
