#ifndef SETUP_H
#define SETUP_H

#include "../core/program.h"
#include "../core/axis.h"
#include "constants.h"

void setupAxes(Program& p) {

  Axis* axisT = &p.axisT;
  axisT->setStepsPerUnit(200 * 2 * 16 / (360*12/61));
  axisT->enabledPin = 8;
  axisT->dirPin = 10;
  axisT->stepPin = 11;
  axisT->limitSwitchPin = 12;
  axisT->setupPins();
  axisT->setReverseMotorDirection(true);

  Axis* axisA = &p.axisA;
  axisA->setStepsPerUnit(200 * 2 * 16 / (360*20/69));
  axisA->enabledPin = 8;
  axisA->dirPin = 7;
  axisA->stepPin = 6;
  axisA->limitSwitchPin = 12;
  axisA->setupPins();

  Axis* axisB = &p.axisB;
  axisB->setStepsPerUnit(200 * 2 * 16 / (360*20/69));
  axisB->enabledPin = 8;
  axisB->dirPin = 7;
  axisB->stepPin = 6;
  axisB->limitSwitchPin = 12;
  axisB->setupPins();

  HorizontalAxis* axisX = &p.axisX;
  axisX->setStepsPerUnit(200 * 2 * 8 / (12.2244*3.1416)); // Diameter from openscad pulley file
  axisX->enabledPin = 8;
  axisX->dirPin = 2;
  axisX->stepPin = 3;
  axisX->limitSwitchPin = 12;
  axisX->setupPins();
  axisX->setMaxPosition(AXIS_X_MAX_POS);

  Axis* axisY = &p.axisY;
  axisY->setStepsPerUnit(200 * 2 * 8 / (2.625*25.4*3.1416));
  axisY->enabledPin = 8;
  axisY->dirPin = 7;
  axisY->stepPin = 6;
  axisY->limitSwitchPin = 12;
  axisY->setupPins();

  ZAxis* axisZ = &p.axisZ;
  axisZ->setStepsPerUnit(200 * 2 * 16 / (360*12/61));
  axisZ->enabledPin = 8;
  axisZ->dirPin = 10;
  axisZ->stepPin = 11;
  axisZ->limitSwitchPin = 12;
  axisZ->setupPins();
  axisZ->setMaxPosition(RAYON);
  axisZ->setReverseMotorDirection(true);
}

#endif
