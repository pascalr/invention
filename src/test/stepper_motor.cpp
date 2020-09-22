#include "test.h"
#include "../core/writer/stream_writer.h"
#include "../core/StepperMotor.h"

#include "../lib/draw_matplotlib.h"
#include "../lib/lib.h"

void testAcceleration() {
  title("Testing Axis::timeToReachDestination");

  unsigned long moveTimeUs = 10 * 1000 * 1000; // 10 seconds
  //double destination = 80.0;
  double destination = 142.0;
  double maxSpeed = 1.5;
  double acceleration = 1.5;
  
  //p.axisV.limitSwitchPin = 12;
 
  StreamWriter writer;
  StepperMotor axis(writer, 'G');
  
  double unitPerTurnY = (2.625*25.4*3.1416 * 13/51);
  axis.setStepsPerUnit(200 * 2 * 16 / unitPerTurnY);
  axis.setStepsPerTurn(200 * 2 * 16);

  axis.setupPins(8,10,11);
  axis.setReverseMotorDirection(true);
  axis.setDefaultMaxSpeed(maxSpeed);
  axis.setAcceleration(acceleration);

  axis.referenceReached();
  axis.setPosition(0);
  
  axis.prepareWorking(0);
  axis.setDestination(destination);

  int nbPoints = 100;
  vector<double> t(nbPoints);
  vector<double> v(nbPoints);
  vector<double> p(nbPoints);

  unsigned long captureInterval = moveTimeUs / nbPoints;
  unsigned long lastCapture = 0;

  for (unsigned long time = 0; time < moveTimeUs; time += 50) {
    axis.handleAxis(time);
    if ((time - lastCapture) > captureInterval) {
      t.push_back(time / 1000000.0);
      v.push_back(axis.getCurrentSpeed() / maxSpeed);
      p.push_back(axis.getPosition() / destination);
      if (axis.getCurrentSpeed() < 0) {debug();}
      lastCapture = time;
    }
  }

  beforeRenderScene();

  drawLines(t,v,"b-");
  drawLines(t,p,"r-");
  show();

  // So it should take one second to reach the destination
}

int main (int argc, char *argv[]) {

  signal(SIGINT, signalHandler);

  testAcceleration();

}
