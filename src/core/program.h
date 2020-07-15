#ifndef PROGRAM_H
#define PROGRAM_H

#include <ctype.h>
#include "axis.h"

class Program {
  public:
    Program(Writer& writer) :
        axisT(writer, 'T'), axisZ(writer, 'Z', axisT),
        baseAxisX(writer, 'Q', axisT), axisX(writer, 'X', baseAxisX, axisT),
        axisA(writer, 'A'), axisY(writer, 'Y'), axisB(writer, 'B') {
    }

    virtual Writer& getWriter() = 0;
    virtual unsigned long getCurrentTime() = 0;
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
      for (int i = 0; motorAxes[i] != 0; i++) {
        motorAxes[i]->stop();
      }
      getWriter() << "Stopped\n";
      isWorking = false; // Maybe not necessary because already told the axes to stop. Anyway it does not hurt..
    }
    
    MotorAxis axisT; // Real one
    ZAxis axisZ; // Virtual one

    BaseXAxis baseAxisX; // Real one
    XAxis axisX; // Virtual one

    MotorAxis axisA;
    MotorAxis axisY;
    MotorAxis axisB;
     
    // These axes actually moves the motors. 
    MotorAxis* motorAxes[10] = {&baseAxisX, &axisY, &axisT, &axisA, &axisB, 0}; // FIXME: CAREFULL WITH SIZE!!!

    // These axes are the ones you can move. mx100, mz100, etc...
    Axis* movingAxes[10] = {&axisX, &axisY, &axisT, &axisZ, &axisA, &axisB, 0}; // FIXME: CAREFULL WITH SIZE!!!
    
    bool isWorking = false;

};

#endif
