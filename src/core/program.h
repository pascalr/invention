#ifndef PROGRAM_H
#define PROGRAM_H

#include <ctype.h>
#include "axis.h"

class Program {
  public:
    Program(Writer& writer) : axisY(writer, 'Y'), axisX(writer, 'X'), axisA(writer, 'A'),
        axisB(writer, 'B'), axisZ(writer, 'Z', &axisX), axisT(writer, 'T') {
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

    VerticalAxis axisY;
    HorizontalAxis axisX;
    Axis axisA;
    Axis axisB;
    ZAxis axisZ;
    Axis axisT;
      
    Axis* axes[10] = {&axisX, &axisY, &axisT, &axisA, &axisB, &axisZ, 0}; // FIXME: CAREFULL WITH SIZE!!!
    //Axis* axes[] = {&axisX, &axisY, &axisT, &axisA, &axisB, &axisZ, 0};
    
    bool isWorking = false;

};

#endif
