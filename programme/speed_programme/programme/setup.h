#ifndef SETUP_H
#define SETUP_H

#include "axis.h"

// TODO: Pass only Axis** axes, then here compare with to name to setup.
void setupAxes(Writer* writer, HorizontalAxis* axisX, VerticalAxis* axisY, Axis* axisT) {

  axisX->setRotationAxis(axisT);
  
  axisT->enabledPin = 8;
  axisT->dirPin = 10;
  axisT->stepPin = 11;
  axisT->limitSwitchPin = 12;

  axisX->enabledPin = 8;
  axisX->dirPin = 2;
  axisX->stepPin = 3;
  axisX->limitSwitchPin = 12;

  axisY->enabledPin = 8;
  axisY->dirPin = 7;
  axisY->stepPin = 6;
  axisY->limitSwitchPin = 12;
  // ***************************************
  
  axisX->setupPins();
  axisY->setupPins();
  axisT->setupPins();

  // Linear axes units are mm. Rotary axes units are degrees.
  // Number of steps per turn of the motor * microstepping / distance per turn
  // The value is multiplied by two because we have to write LOW then HIGH for one step
  axisX->stepsPerUnit = 200 * 2 * 8 / (1.25*25.4*3.1416);
  axisY->stepsPerUnit = 200 * 2 * 8 / (2.625*25.4*3.1416);
  axisT->stepsPerUnit = 200 * 2 * 8 / (360*12/61);
  
  axisX->setMotorEnabled(false);
  axisY->setMotorEnabled(false);
  axisT->setMotorEnabled(false);
}

#endif
