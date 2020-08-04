#ifndef SETUP_H
#define SETUP_H

#include "../core/program.h"
#include "../core/axis.h"
#include "constants.h"
#include <float.h>

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
  p.baseAxisX.setDefaultMaxSpeed(1.5);
  p.baseAxisX.setAcceleration(1.5);

  p.axisY.setStepsPerUnit(200 * 2 * 16 / (2.625*25.4*3.1416) * (51/13));
  p.axisY.setStepsPerTurn(200 * 2 * 16);
  //p.axisY.limitSwitchPin = 12;
  p.axisY.setupPins(8,7,6);
  //p.axisY.setMaxPosition(AXIS_Y_MAX_POS);
  p.axisY.setMaxPosition(900); // FIXME: stepsPerUnit is not OK
  p.axisY.setDefaultMaxSpeed(1.5);
  p.axisY.setAcceleration(1.5);
  p.axisY.setReverseMotorDirection(true);

}

#endif
