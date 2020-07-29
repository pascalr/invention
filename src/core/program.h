#ifndef PROGRAM_H
#define PROGRAM_H

#include <ctype.h>
#include "axis.h"
#include "StepperMotor.h"
#include "DCMotor.h"

#define NUMBER_OF_MOTORS 6
#define NUMBER_OF_AXES 7

class Program {
  public:
    Program(Writer& writer) :
        axisT(writer, 'T'), axisZ(writer, 'Z', axisT),
        baseAxisX(writer, 'Q', axisT), axisX(writer, 'X', baseAxisX, axisT),
        axisA(writer, 'A'), axisY(writer, 'Y'), axisB(writer, 'B'), axisR(writer, 'R') {
    }

    virtual Writer& getWriter() = 0;

    virtual void sleepMs(int time) = 0;
    virtual bool inputAvailable() = 0;
    virtual bool getInput(char* buf, int size) = 0;
    virtual int getByte() = 0;
    
    char getChar() {
      int receivedByte;
      while ((receivedByte = getByte()) < 0) {
        sleepMs(1);
      }
      return (char) receivedByte;
    }

    void stopMoving() {
      for (int i = 0; motors[i] != 0; i++) {
        motors[i]->stop();
      }
      getWriter() << "Stopped\n";
      isWorking = false; // Maybe not necessary because already told the axes to stop. Anyway it does not hurt..
    }
    
    StepperMotor axisT; // Real one
    ZAxis axisZ; // Virtual one

    BaseXAxis baseAxisX; // Real one
    XAxis axisX; // Virtual one

    StepperMotor axisA;
    StepperMotor axisY;
    StepperMotor axisB;

    DCMotor axisR;
     
    // These axes actually moves the motors. 
    Motor* motors[NUMBER_OF_MOTORS+1] = {&baseAxisX, &axisY, &axisT, &axisA, &axisB, &axisR, 0}; // FIXME: CAREFULL WITH SIZE!!!

    // These axes are the ones you can move. mx100, mz100, etc...
    Axis* movingAxes[NUMBER_OF_AXES+1] = {&axisX, &axisY, &axisT, &axisZ, &axisA, &axisB, &axisR, 0}; // FIXME: CAREFULL WITH SIZE!!!
    
    bool isWorking = false;
    
    virtual unsigned long getCurrentTime() = 0;
};



#endif
