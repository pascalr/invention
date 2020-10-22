#ifndef _PROGRAM_H
#define _PROGRAM_H


#include <ctype.h>
#include "Motor.h"
#include "StepperMotor.h"
#include "DCMotor.h"
#include "reader/reader.h"
#include "../utils/constants.h"
#include <float.h>

#define NUMBER_OF_MOTORS 6
#define NUMBER_OF_AXES 7

// Way too big but at least I wont have to worry about that.
// If I run out of space some time bring this down.
// Could be as low as 52.
#define MAX_INPUT_LENGTH 256

class Program {
  public:
    Program(Writer& writer, Reader& reader) :
        axisT(writer, 'T'), axisH(writer, 'H'),
        axisV(writer, 'V'),
        // The movement of axis R refers to the space between the two jaws (hence multiplied by 2)
        axisR(writer, 'R', 8, 2.0*(0.186*25.4*13)/90.0) {
    }

    virtual Writer& getWriter() = 0;
    virtual Reader& getReader() = 0;
    
    virtual void sleepMs(int time) = 0;

    char getChar();

    char* getInputLine();

    void stopMoving();
    
    MotorT axisT;
    StepperMotor axisH;
    StepperMotor axisV;

    DCMotor axisR;
     
    // These axes actually moves the motors. 
    Motor* motors[NUMBER_OF_MOTORS+1] = {&axisH, &axisV, &axisT, &axisR, 0}; // FIXME: CAREFULL WITH SIZE!!!
    StepperMotor* steppers[4] = {&axisH, &axisV, &axisT}; // FIXME: CAREFULL WITH SIZE!!!

    bool isWorking = false;
    
    virtual unsigned long getCurrentTime() = 0;
    char input[MAX_INPUT_LENGTH];
};

void setupAxes(Program& p);

#endif
