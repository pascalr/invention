#ifndef SETUP_H
#define SETUP_H

#include "axis.h"
#include "common.h"

// TODO: Pass only Axis** axes, then here compare with to name to setup.
void setupAxes(Writer* writer, Axis** axes) {

  Axis* axisX = axisByLetter(axes, 'X');
  Axis* axisY = axisByLetter(axes, 'Y');
  Axis* axisT = axisByLetter(axes, 'T');
  Axis* axisA = axisByLetter(axes, 'A');
  Axis* axisB = axisByLetter(axes, 'B');

  if (axisT) {
    axisT->setStepsPerUnit(200 * 2 * 16 / (360*12/61));
    axisT->enabledPin = 8;
    axisT->dirPin = 10;
    axisT->stepPin = 11;
    axisT->limitSwitchPin = 12;
    axisT->setupPins();
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
    axisX->setStepsPerUnit(200 * 2 * 8 / (1.25*25.4*3.1416));
    ((HorizontalAxis*) axisX)->setRotationAxis(axisT);
    axisX->enabledPin = 8;
    axisX->dirPin = 2;
    axisX->stepPin = 3;
    axisX->limitSwitchPin = 12;
    axisX->setupPins();
  }

  if (axisY) {
    axisY->setStepsPerUnit(200 * 2 * 8 / (2.625*25.4*3.1416));
    axisY->enabledPin = 8;
    axisY->dirPin = 7;
    axisY->stepPin = 6;
    axisY->limitSwitchPin = 12;
    axisY->setupPins();
  }
}

#endif
