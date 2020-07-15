#include "test.h"
#include "../utils/io_common.h"
#include "../config/constants.h"

#include "../core/axis.h"
#include "../core/fake_program.h"
#include "../config/setup.h"

using namespace std;

void testCalculateNextStep() {
  title("Testing Axis::calculateNextStep");

  FakeProgram p;
  setupAxes(p);

  unsigned long startTime = 0;

  MotorAxis& axis = p.axisY;
  axis.referenceReached();
  axis.setAcceleration(10); // tour/s^2
  axis.setMaxSpeed(10); // tour/s
  axis.setPosition(0);
  
  axis.prepare(startTime);

  double turns = 100; // A lot of turns to be sure the axis has reached full speed in the middle
  double units = turns * axis.getStepsPerTurn() / axis.getStepsPerUnit();

  axis.setDestination(units);
  
  unsigned long middleTime = axis.timeToReachDestinationUs()/2;
  
  unsigned long fullSpeedDelay = ((unsigned long)(1000000.0 / (axis.getMaxSpeed() * axis.getStepsPerTurn()))); // us
 
  assertNearby("At first, delay should be maximal (slow speed first)", MAX_STEP_DELAY, axis.calculateNextStepDelay(startTime));
  assertNearby("In the middle, delay should be full speed", fullSpeedDelay, axis.calculateNextStepDelay(middleTime));
  assertNearby("At the end, delay should be maximal", MAX_STEP_DELAY, axis.calculateNextStepDelay(middleTime*2));
}

void testTimeToReachDestination() {
  title("Testing Axis::timeToReachDestination");
  
  FakeProgram p;
  setupAxes(p);

  MotorAxis& axis = p.axisY;
  axis.referenceReached();
  axis.setAcceleration(10); // tour/s^2
  axis.setMaxSpeed(10); // tour/s
  axis.setPosition(0);

  double turns = 10;
  double units = turns * axis.getStepsPerTurn() / axis.getStepsPerUnit();

  axis.setDestination(units);

  // The axis should not have enough time to get to full acceleration.
  // So no constant speed.
  double t = 2*sqrt(2*(turns/2)/axis.getAcceleration());

  assertNearby("timeToReachDestination", t*1000000, axis.timeToReachDestinationUs());
  assertNearby("timeToStartDecelerating", t*1000000/2, axis.m_time_to_start_decelerating_us);

  // So it should take one second to reach the destination
}

void testSetDestination() {
  
  FakeProgram p;
  setupAxes(p);

  p.axisY.setAcceleration(100); // tour/s^2
  p.axisY.setMaxSpeed(100); // tour/s

  // So it should take one second to reach the destination
}

int main (int argc, char *argv[]) {

  signal(SIGINT, signalHandler);

  testCalculateNextStep();
  testTimeToReachDestination();
  testSetDestination();

}
