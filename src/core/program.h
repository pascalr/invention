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
      
    Axis* axes[10] = {&axisX, &axisY, &axisT, &axisA, &axisB, &axisZ, NULL}; // FIXME: CAREFULL WITH SIZE!!!
    //Axis* axes[] = {&axisX, &axisY, &axisT, &axisA, &axisB, &axisZ, NULL};
    
    bool isWorking = false;

    //friend Writer& operator<<(Writer& writer, const char* theString);

    /*void serialize(Writer& out) {
      out << "{";
      for (int i = 0; axes[i] != NULL; i++) {
        out << "\"axis_" << axes[i]->name << "\": ";
        out << axes[i];
        if (axes[i+1] != NULL) {
          out << ", ";
        }
      }
      out << "}";
    }*/
};

/*Writer& operator<<(Writer& out, const Program &p) {
  out << "{";
  for (int i = 0; p.axes[i] != NULL; i++) {
    out << "\"axis_" << p.axes[i]->name << "\": ";
    p.axes[i]->serialize(out);
    if (p.axes[i+1] != NULL) {
      out << ", ";
    }
  }
  return out << "}";
}*/

#endif
