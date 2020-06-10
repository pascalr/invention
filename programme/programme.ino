#include "axis.h"
#include "setup.h"
#include "common.h" 

// CONSTANTS
#define SLOW_SPEED_DELAY 2000
#define FAST_SPEED_DELAY 100

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

    void doPrint(char val) {
      Serial.print(val);
    }

    void doPrintLn(const char* theString) {
      Serial.println(theString);
    }

    void doPrintLn(double val) {
      Serial.println(val);
    }
};

ArduinoWriter writer = ArduinoWriter();
Axis axisT = Axis(&writer, 'T');
VerticalAxis axisY = VerticalAxis(&writer, 'Y');
HorizontalAxis axisX = HorizontalAxis(&writer, 'X');
Axis axisA = Axis(&writer, 'A');
Axis axisB = Axis(&writer, 'B');

Axis* axes[] = {&axisX, &axisY, &axisT, &axisA, &axisB, NULL};

char input[256];

int inputCursor = 0;
byte inputSize = 0;

void setup() {

  //Initiate Serial communication.
  Serial.begin(115200);
  Serial.println("Setup...");

  setupAxes(&writer, axes);
  
  Serial.println("Done");
}

void loop() {
  
  if (Serial.available() > 0) {

    // If we are done doing the commands, get another one.
    if (inputCursor >= inputSize) { 
      inputCursor = 0;
      inputSize = Serial.readBytes(input, 254);
      
      if (input[inputSize-1] == '\n') {inputSize--;}
      if (input[inputSize-1] == '\r') {inputSize--;}
      
      input[inputSize] = 0; // Add the final 0 to end the C string
      
    } else {
      // Should not received another command exept stop.
      int cmd = Serial.read();
      if (cmd == 's' || cmd == 'S') {
        for (int i = 0; axes[i] != NULL; i++) {
          axes[i]->stop();
        }
      } else if (cmd == '?') {
        for (int i = 0; axes[i] != NULL; i++) {
          printDebugAxis(axes[i], &writer);
        }
      } else {
        Serial.println("Error received command while previous was not finished.");
      }
    }
    
  }

  unsigned long currentTime = micros();
  bool stillWorking = false;
  for (int i = 0; axes[i] != NULL; i++) {
    stillWorking = stillWorking || axes[i]->handleAxis(currentTime);
  }

  if (!stillWorking && inputCursor < inputSize) {
    inputCursor = parseInput(input, &writer, axes, inputCursor);
  }
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
