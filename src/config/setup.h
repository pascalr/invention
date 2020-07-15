#ifndef SETUP_H
#define SETUP_H

#include "../core/program.h"
#include "../core/axis.h"
#include "constants.h"

void setupAxes(Program& p) {

  p.axisT.setStepsPerUnit(200 * 2 * 16 / (360*12/61));
  p.axisT.setStepsPerTurn(200 * 2 * 16);
  p.axisT.enabledPin = 8;
  p.axisT.dirPin = 10;
  p.axisT.stepPin = 11;
  p.axisT.limitSwitchPin = 12;
  p.axisT.setupPins();
  p.axisT.setReverseMotorDirection(true);
  p.axisT.setMinPosition(-45);
  p.axisT.setMaxPosition(225);
  p.axisT.setMaxSpeed(0.2);
  p.axisT.setAcceleration(0.05);

  p.axisZ.setMaxPosition(RAYON);
  p.axisZ.setDestination(0.0);

  p.baseAxisX.setStepsPerUnit(200 * 2 * 8 / (12.2244*3.1416)); // Diameter from openscad pulley file
  p.baseAxisX.setStepsPerTurn(200 * 2 * 8);
  p.baseAxisX.enabledPin = 8;
  p.baseAxisX.dirPin = 2;
  p.baseAxisX.stepPin = 3;
  p.baseAxisX.limitSwitchPin = 12;
  p.baseAxisX.setupPins();
  p.baseAxisX.setMaxPosition(AXIS_X_MAX_POS);
  p.baseAxisX.setMaxSpeed(1.5);
  p.baseAxisX.setAcceleration(0.2);

  p.axisX.setMaxPosition(AXIS_X_MAX_POS);
  p.axisX.setDestination(RAYON);

  p.axisA.setStepsPerUnit(200 * 2 * 16 / (360*20/69));
  p.axisA.setStepsPerTurn(200 * 2 * 16);
  p.axisA.enabledPin = 8;
  p.axisA.dirPin = 7;
  p.axisA.stepPin = 6;
  p.axisA.limitSwitchPin = 12;
  p.axisA.setupPins();

  p.axisB.setStepsPerUnit(200 * 2 * 16 / (360*20/69));
  p.axisB.setStepsPerTurn(200 * 2 * 16);
  p.axisB.enabledPin = 8;
  p.axisB.dirPin = 7;
  p.axisB.stepPin = 6;
  p.axisB.limitSwitchPin = 12;
  p.axisB.setupPins();

  p.axisY.setStepsPerUnit(200 * 2 * 8 / (2.625*25.4*3.1416));
  p.axisY.setStepsPerTurn(200 * 2 * 8);
  p.axisY.enabledPin = 8;
  p.axisY.dirPin = 7;
  p.axisY.stepPin = 6;
  p.axisY.limitSwitchPin = 12;
  p.axisY.setupPins();

  
}

#endif
