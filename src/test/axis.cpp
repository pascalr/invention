#include "test.h"

#include "axis.h"

using namespace std;

void testTimeToReachDestination() {
  title("Testing Axis::timeToReachDestination");
  
  FakeProgram p;
  setupAxes(p);

  p.axisY.setAcceleration(100); // tour/s^2
  p.axisY.setRpmMax(100); // tour/s
  p.axisY.setPosition(0);
  p.axisY.setDestination(100);
  
  double dx = p.axisY.getDestination() - p.axisY.getPosition();
  double t = sqrt(2*dx/)

  // So it should take one second to reach the destination
}

void testSetDestination() {
  
  FakeProgram p;
  setupAxes(p);

  p.axisY.setAcceleration(100); // tour/s^2
  p.axisY.setRpmMax(100); // tour/s

  // So it should take one second to reach the destination
}

int main (int argc, char *argv[]) {

  signal(SIGINT, signalHandler);

  testTimeToReachDestination();
  testSetDestination();

}
