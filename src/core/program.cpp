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
  p.axisR.setReverseMotorDirection(false); // FIXME: THIS IS MODIFIED IN moveSpoon and moveGrip...
  // min and max positions depends on the tool, sometimes no limits (mixer), sometimes yes (gripper)

  //p.axisT.microsteps = 16;
  p.axisT.setStepsPerUnit(200 * 2 * 16 / (360*12/61));
  p.axisT.steps_per_turn = 200 * 2 * 16;
  //p.axisT.limitSwitchPin = 12;
  p.axisT.setupPins(8,2,3);
  p.axisT.setReverseMotorDirection(true);
  p.axisT.percent_p = 0.4;
  p.axisT.min_delay = 100;
  p.axisT.max_delay = 2000;

  //p.axisH.microsteps = 8;
  p.axisH.setStepsPerUnit(200 * 2 * 8 / (12.2244*3.1416)); // Diameter from openscad pulley file
  p.axisH.steps_per_turn = 200 * 2 * 8;
  //p.axisH.limitSwitchPin = 12;
  p.axisH.setupPins(8,10,11);
  p.axisH.setReverseMotorDirection(true);
  p.axisH.referencer = LimitSwitchReferencer(4);
  p.axisH.percent_p = 0.3;
  p.axisH.min_delay = 100;
  p.axisH.max_delay = 2000;
  p.axisH.reference_speed_rpm = 30;

  double unitPerTurnY = (2.625*25.4*3.1416 * 13/51);
  p.axisV.setStepsPerUnit(200 * 2 * 32 / unitPerTurnY);
  p.axisV.steps_per_turn = 200 * 2 * 32;
  //p.axisV.limitSwitchPin = 12;
  p.axisV.setupPins(8,7,6);
  p.axisV.setReverseMotorDirection(true);
  p.axisV.referencer = LimitSwitchReferencer(12);
  p.axisV.percent_p = 0.3;
  p.axisV.min_delay = 40;
  p.axisV.max_delay = 1000;
  p.axisV.reference_speed_rpm = 30;

}
