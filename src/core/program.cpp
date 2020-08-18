#include "program.h"

#include <ctype.h>
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

char Program::getChar() {
  int receivedByte;
  while ((receivedByte = getReader().getByte()) < 0) {
    sleepMs(1);
  }
  return (char) receivedByte;
}

char* Program::getInputLine() {
  int i = 0;
  char ch;
  while ((ch = getChar()) != '\n') {
    input[i] = ch;
    i++;
  }
  input[i] = '\0';
  return input;
}

void Program::stopMoving() {
  for (int i = 0; motors[i] != 0; i++) {
    motors[i]->stop();
  }
  getWriter() << "Stopped\n";
  isWorking = false; // Maybe not necessary because already told the axes to stop. Anyway it does not hurt..
}

void setupAxes(Program& p) {

  p.axisR.setupPins(8,9,5);
  p.axisR.setReverseMotorDirection(true);
  // min and max positions depends on the tool, sometimes no limits (mixer), sometimes yes (gripper)
  p.axisR.setMinPosition(-DBL_MAX);
  p.axisR.setMaxPosition(DBL_MAX);

  p.axisT.setStepsPerUnit(200 * 2 * 16 / (360*12/61));
  p.axisT.setStepsPerTurn(200 * 2 * 16);
  //p.axisT.limitSwitchPin = 12;
  p.axisT.setupPins(8,10,11);
  p.axisT.setReverseMotorDirection(true);
  p.axisT.setMinPosition(-45);
  p.axisT.setMaxPosition(225);
  p.axisT.setDefaultMaxSpeed(0.2);
  p.axisT.setAcceleration(0.2);

  p.baseAxisX.setStepsPerUnit(200 * 2 * 8 / (12.2244*3.1416)); // Diameter from openscad pulley file
  p.baseAxisX.setStepsPerTurn(200 * 2 * 8);
  //p.baseAxisX.limitSwitchPin = 12;
  p.baseAxisX.setupPins(8,2,3);
  p.baseAxisX.setMaxPosition(AXIS_X_MAX_POS);
  p.baseAxisX.setMinPosition(0);
  p.baseAxisX.setDefaultMaxSpeed(1.5);
  p.baseAxisX.setAcceleration(1.5);

  double unitPerTurnY = (2.625*25.4*3.1416 * 13/51);
  p.axisY.setStepsPerUnit(200 * 2 * 16 / unitPerTurnY);
  p.axisY.setStepsPerTurn(200 * 2 * 16);
  //p.axisY.limitSwitchPin = 12;
  p.axisY.setupPins(8,7,6);
  //p.axisY.setMaxPosition(AXIS_Y_MAX_POS);
  p.axisY.setMaxPosition(900); // FIXME: stepsPerUnit is not OK
  p.axisY.setMinPosition(-900); // tmp
  p.axisY.setDefaultMaxSpeed(1.5);
  p.axisY.setAcceleration(1.5);
  p.axisY.setReverseMotorDirection(true);
  p.axisY.referencer = LimitSwitchReferencer(12);

}
