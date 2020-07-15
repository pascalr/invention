#include "test.h"
#include "../utils/io_common.h"
#include "../config/constants.h"

#include "../core/axis.h"
#include "../core/fake_program.h"
#include "../config/setup.h"

using namespace std;

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
  
  //double dx = axis.getDestination() - axis.getPosition();
  //double t = sqrt(2*dx/axis.getAccelerationInUnits());
  double t = sqrt(2*turns/axis.getAcceleration());

  assertNearby("t", t*1000000, axis.timeToReachDestinationUs());

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

  testTimeToReachDestination();
  testSetDestination();

}
