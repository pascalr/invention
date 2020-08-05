#ifndef PROGRAM_H
#define PROGRAM_H

#include <ctype.h>
#include "axis.h"
#include "StepperMotor.h"
#include "DCMotor.h"
#include "reader/reader.h"

#define NUMBER_OF_MOTORS 6
#define NUMBER_OF_AXES 7

// Way too big but at least I wont have to worry about that.
// If I run out of space some time bring this down.
// Could be as low as 52.
#define MAX_INPUT_LENGTH 256

class Program {
  public:
    Program(Writer& writer, Reader& reader) :
        axisT(writer, 'T'), baseAxisX(writer, 'X', axisT),
        axisY(writer, 'Y'), axisR(writer, 'R') {
    }

    virtual Writer& getWriter() = 0;
    virtual Reader& getReader() = 0;
    
    virtual void sleepMs(int time) = 0;

    char getChar() {
      int receivedByte;
      while ((receivedByte = getReader().getByte()) < 0) {
        sleepMs(1);
      }
      return (char) receivedByte;
    }

    char* getInputLine() {
      int i = 0;
      char ch;
      while ((ch = getChar()) != '\n') {
        input[i] = ch;
        i++;
      }
      input[i] = '\0';
      return input;
    }

    void stopMoving() {
      for (int i = 0; motors[i] != 0; i++) {
        motors[i]->stop();
      }
      getWriter() << "Stopped\n";
      isWorking = false; // Maybe not necessary because already told the axes to stop. Anyway it does not hurt..
    }
    
    MotorT axisT; // Real one

    BaseXAxis baseAxisX; // Real one

    StepperMotor axisY;

    DCMotor axisR;
     
    // These axes actually moves the motors. 
    Motor* motors[NUMBER_OF_MOTORS+1] = {&baseAxisX, &axisY, &axisT, &axisR, 0}; // FIXME: CAREFULL WITH SIZE!!!

    bool isWorking = false;
    
    virtual unsigned long getCurrentTime() = 0;
    char input[MAX_INPUT_LENGTH];
};



#endif
