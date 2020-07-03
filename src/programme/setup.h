#ifndef SETUP_H
#define SETUP_H

#include "axis.h"
#include "common.h"

#define AXIS_X_MAX_POS 640.0 // mm
#define AXIS_Z_MAX_POS RAYON

#define ARM_WIDTH 111.0 // mm
#define ARM_LENGTH 370.0 // mm

#define ARMOIRE_WIDTH 1016.0 // mm
#define ARMOIRE_DEPTH 609.6 // mm
#define ARMOIRE_HEIGHT 1905.0 // mm

#define OFFSET_X 164.0 // mm, the distance between the sidewall and x = 0
#define OFFSET_Z 160.0 // mm, the distance between the back and z = 0

// TODO: Pass only Axis** axes, then here compare with to name to setup.
void setupAxes(Writer* writer, Axis** axes) {

  Axis* axisX = axisByLetter(axes, 'X');
  Axis* axisY = axisByLetter(axes, 'Y');
  Axis* axisT = axisByLetter(axes, 'T');
  Axis* axisA = axisByLetter(axes, 'A');
  Axis* axisB = axisByLetter(axes, 'B');
  Axis* axisZ = axisByLetter(axes, 'Z');

  if (axisT) {
    axisT->setStepsPerUnit(200 * 2 * 16 / (360*12/61));
    axisT->enabledPin = 8;
    axisT->dirPin = 10;
    axisT->stepPin = 11;
    axisT->limitSwitchPin = 12;
    axisT->setupPins();
    axisT->setReverseMotorDirection(true);
  }

  if (axisA) {
    axisA->setStepsPerUnit(200 * 2 * 16 / (360*20/69));
    axisA->enabledPin = 8;
    axisA->dirPin = 7;
    axisA->stepPin = 6;
    axisA->limitSwitchPin = 12;
    axisA->setupPins();
  }

  if (axisB) {
    axisB->setStepsPerUnit(200 * 2 * 16 / (360*20/69));
    axisB->enabledPin = 8;
    axisB->dirPin = 7;
    axisB->stepPin = 6;
    axisB->limitSwitchPin = 12;
    axisB->setupPins();
  }

  if (axisX) {
    axisX->setStepsPerUnit(200 * 2 * 8 / (12.2244*3.1416)); // Diameter from openscad pulley file
    axisX->enabledPin = 8;
    axisX->dirPin = 2;
    axisX->stepPin = 3;
    axisX->limitSwitchPin = 12;
    axisX->setupPins();
    axisX->setMaxPosition(AXIS_X_MAX_POS);
  }

  if (axisY) {
    axisY->setStepsPerUnit(200 * 2 * 8 / (2.625*25.4*3.1416));
    axisY->enabledPin = 8;
    axisY->dirPin = 7;
    axisY->stepPin = 6;
    axisY->limitSwitchPin = 12;
    axisY->setupPins();
  }

  if (axisZ) {
    axisZ->setStepsPerUnit(200 * 2 * 16 / (360*12/61));
    axisZ->enabledPin = 8;
    axisZ->dirPin = 10;
    axisZ->stepPin = 11;
    axisZ->limitSwitchPin = 12;
    axisZ->setupPins();
    axisZ->setMaxPosition(RAYON);
    axisZ->setReverseMotorDirection(true);
  }
}

#endif
