#ifndef SETUP_H
#define SETUP_H

#include "../core/program.h"
#include "../core/axis.h"
#include "constants.h"

void setupAxes(Program& p) {

  p.axisR.setupPins(8,9,5);
  //p.axisR.setReverseMotorDirection(true);
  // min and max positions depends on the tool, sometimes no limits (mixer), sometimes yes (gripper)
  //p.axisR.setMinPosition(MINUS_INFINITY);
  //p.axisR.setMaxPosition(INFINITY);

  p.axisT.setStepsPerUnit(200 * 2 * 16 / (360*12/61));
  p.axisT.setStepsPerTurn(200 * 2 * 16);
  //p.axisT.limitSwitchPin = 12;
  p.axisT.setupPins(8,10,11);
  p.axisT.setReverseMotorDirection(true);
  p.axisT.setMinPosition(-45);
  p.axisT.setMaxPosition(225);
  p.axisT.setDefaultMaxSpeed(0.2);
  p.axisT.setAcceleration(0.2);

  p.axisZ.setMaxPosition(RAYON);
  p.axisZ.setDestination(0.0);

  p.baseAxisX.setStepsPerUnit(200 * 2 * 8 / (12.2244*3.1416)); // Diameter from openscad pulley file
  p.baseAxisX.setStepsPerTurn(200 * 2 * 8);
  //p.baseAxisX.limitSwitchPin = 12;
  p.baseAxisX.setupPins(8,2,3);
  p.baseAxisX.setMaxPosition(AXIS_X_MAX_POS);
  p.baseAxisX.setDefaultMaxSpeed(1.5);
  p.baseAxisX.setAcceleration(1.5);

  p.axisX.setMaxPosition(AXIS_X_MAX_POS);
  p.axisX.setDestination(RAYON);

  p.axisA.setStepsPerUnit(200 * 2 * 16 / (360*20/69));
  p.axisA.setStepsPerTurn(200 * 2 * 16);
  //p.axisA.limitSwitchPin = 12;
  p.axisA.setupPins(8,7,6);

  p.axisB.setStepsPerUnit(200 * 2 * 16 / (360*20/69));
  p.axisB.setStepsPerTurn(200 * 2 * 16);
  //p.axisB.limitSwitchPin = 12;
  p.axisB.setupPins(8,7,6);

  p.axisY.setStepsPerUnit(200 * 2 * 8 / (2.625*25.4*3.1416));
  p.axisY.setStepsPerTurn(200 * 2 * 8);
  //p.axisY.limitSwitchPin = 12;
  p.axisY.setupPins(8,7,6);
  
}

#endif
