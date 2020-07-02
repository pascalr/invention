#include <iostream>
#include <cstring>
#include <math.h>

#include "../axis.h"
#include "../setup.h"
#include "../common.h"

#include "matplotlibcpp.h"
      
#include <chrono> // for sleep
#include <thread> // for sleep

#include "console_writer.h"

#include <signal.h>

using namespace std;
namespace plt = matplotlibcpp;

void signalHandler( int signum ) {
   cout << "Interrupt signal (" << signum << ") received.\n";

   exit(signum);
}

int main (int argc, char *argv[]) {

  signal(SIGINT, signalHandler);

  ConsoleWriter writer = ConsoleWriter();
  Axis axisT = Axis(writer, 'T');
  VerticalAxis axisY = VerticalAxis(writer, 'Y');
  HorizontalAxis axisX = HorizontalAxis(writer, 'X');
  Axis axisA = Axis(writer, 'A');
  Axis axisB = Axis(writer, 'B');
  ZAxis axisZ = ZAxis(writer, 'Z', &axisX);

  Axis* axes[] = {&axisX, &axisY, &axisT, &axisA, &axisB, &axisZ, NULL};
  setupAxes(&writer, axes);

  while (true) {
    cout << ">> ";
    string cmd;
    cin >> cmd;

    if (cmd == "exit" || cmd == "quit" || cmd == "q")
      return 0;

    unsigned long currentTime = 0;

    for (int i = 0; axes[i] != NULL; i++) {
      axes[i]->prepare(currentTime);
    }

    parseInput(cmd.c_str(), &writer, axes, 0);

    for (int i = 0; axes[i] != NULL; i++) {
      axes[i]->afterInput();
    }

    bool stillWorking = true;
    while (stillWorking) {
      stillWorking = false;
      for (int i = 0; axes[i] != NULL; i++) {
        stillWorking = stillWorking || axes[i]->handleAxis(currentTime);
      }
      if (currentTime % 200000 == 0 || !stillWorking) {

        HorizontalAxis* axisX = (HorizontalAxis*)axisByLetter(axes, 'X');
        Axis* axisZ = axisByLetter(axes, 'Z');
        
        plt::clf();
    
        vector<double> x(1);
        vector<double> z(1);
        
        x[0] = axisX->getPosition();
        z[0] = axisZ->getPosition();
        plt::plot(x,z,"ro");

        x.push_back(axisX->getPosition() - axisX->getDeltaPosition());
        z.push_back(0.0);
        plt::plot(x,z,"b-");

        plt::xlim(0.0, axisX->getMaxPosition());
        plt::ylim(0.0, axisZ->getMaxPosition());

	  		plt::title("Position du bras");
	  		plt::pause(0.01);
      }
      currentTime++;
    }
  }

  return 0;
}
 
/*
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
          axes[i]->serialize();
        }
      } else {
        Serial.print("Error received command while previous was not finished: ");
        Serial.println(cmd);
      }
    }
    
  }

  unsigned long currentTime = micros();
  bool stillWorking = false;
  for (int i = 0; axes[i] != NULL; i++) {
    stillWorking = stillWorking || axes[i]->handleAxis(currentTime);
  }

  if (!stillWorking && inputCursor < inputSize) {
    for (int i = 0; axes[i] != NULL; i++) {
     axes[i]->prepare(currentTime);
    }
    inputCursor = parseInput(input, &writer, axes, inputCursor);
    for (int i = 0; axes[i] != NULL; i++) {
     axes[i]->afterInput();
    }
  }
}

 https://arduino.stackexchange.com/questions/1013/how-do-i-split-an-incoming-string
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
