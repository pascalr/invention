#include "axis.h"
#include "setup.h"
#include "common.h" 

// CONSTANTS
#define SLOW_SPEED_DELAY 2000
#define FAST_SPEED_DELAY 100

#define MAX_INPUT_CHUNK_SIZE 254

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

Writer* writer;
Axis* axisT;
VerticalAxis* axisY;
HorizontalAxis* axisX;
Axis* axisA;
Axis* axisB;

Axis* axes[10];

void setup() {

  //Initiate Serial communication.
  //Serial.begin(9600);
  Serial.begin(115200);

  //delay(100); // Wait before printing otherwise it outputs weird characters at the beginning somtetimes.
  
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
    int size = Serial.readBytes(input, MAX_INPUT_CHUNK_SIZE);
    input[size] = 0; // Add the final 0 to end the C string
    if (input[size-1] == '\n') {input[size-1] = 0;}

    Serial.println(size);

    parseInput(input, writer, axes);
  }

  for (int i = 0; axes[i] != NULL; i++) {
    axes[i]->handleAxis(currentTime);
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
