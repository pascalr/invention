#include "test.h"
#include "../core/writer/stream_writer.h"
#include "../core/StepperMotor.h"

#include "../lib/draw_matplotlib.h"
#include "../lib/lib.h"

void testAcceleration() {
  title("Testing Axis::timeToReachDestination");

  unsigned long moveTimeUs = 20 * 1000 * 1000; // 10 seconds
  //double destination = 42.0;
  double destination = 142.0;
  double maxSpeed = 1.5;
  double acceleration = 1.5;
  
  //p.axisV.limitSwitchPin = 12;
 
  StreamWriter writer;
  StepperMotor axis(writer, 'G');
  
  double unitPerTurnY = (2.625*25.4*3.1416 * 13/51);
  axis.setStepsPerUnit(200 * 2 * 16 / unitPerTurnY);
  axis.setStepsPerTurn(200 * 2 * 16);

  //axis.delay_pp = -0.1;
  //axis.delay_pp = -0.03;
  axis.min_delay = 100;
  axis.max_delay = 5000;
  //axis.min_delay_p = -20;
  axis.percent_p = 0.2;

  axis.setupPins(8,10,11);
  axis.setReverseMotorDirection(true);
  axis.setDefaultMaxSpeed(maxSpeed);
  axis.setAcceleration(acceleration);

  axis.referenceReached();
  axis.setPosition(0);
  
  axis.prepareWorking(0);
  axis.setDestination(destination);

  int nbPoints = 500;
  vector<double> t; // time
  vector<double> s; // step
  vector<double> v; // speed
  vector<double> p; // position

  vector<double> d;
  vector<double> d_p;

  unsigned long captureInterval = moveTimeUs / nbPoints;
  unsigned long lastCapture = 0;

  bool jobFinished = false;
  double timeFinishJobS = 0.0;

  for (unsigned long time = 0; time < moveTimeUs; time += 50) {

    bool wasFinished = jobFinished;
    jobFinished = !axis.handleAxis(time);

    if (!wasFinished && jobFinished) {
      timeFinishJobS = time / 1000000.0;
    }

    if ((time - lastCapture) > captureInterval) {
      s.push_back(axis.m_position_steps);
      t.push_back(time / 1000000.0);
      //v.push_back((1/axis.delay)*axis.min_delay);
      p.push_back(axis.getPosition());
      d.push_back(axis.debug_delay);
      //d_p.push_back(axis.delay_p);
      lastCapture = time;
      debug();
    }
  }

  cout << "Distance accelerating: " << axis.distance_accelerating << " steps." << endl;
  cout << "Time stopped accelerating: " << axis.debug_time_finished_accelerating_s << " s." << endl;
  cout << "Time started decelerating: " << axis.time_started_decelerating << " s." << endl;
  cout << "Time job finished : " << timeFinishJobS << " s." << endl;

  //cout << "In phase 1 for " << axis.debug_steps_1 << " steps." << endl;
  //cout << "In phase 2 for " << axis.debug_steps_2 - axis.debug_steps_1 << " steps." << endl;
  //cout << "In phase 3 for " << axis.debug_steps_3 - axis.debug_steps_2 << " steps." << endl;
  //cout << "In phase 4 for " << axis.debug_steps_4 - axis.debug_steps_3 << " steps." << endl;
  //cout << "In phase 5 for " << axis.debug_steps_5 - axis.debug_steps_4 << " steps." << endl;
  //cout << "In phase 6 for " << axis.debug_steps_6 - axis.debug_steps_5 << " steps." << endl;
  //cout << "In phase 7 for " << axis.debug_steps_7 - axis.debug_steps_6 << " steps." << endl;
  //cout << "Phase 2 start time: " << axis.debug_time_1 / 1000000.0 << endl;
  //cout << "Phase 3 start time: " << axis.debug_time_2 / 1000000.0 << endl;
  //cout << "Phase 4 start time: " << axis.debug_time_3 / 1000000.0 << endl;
  //cout << "Phase 5 start time: " << axis.debug_time_4 / 1000000.0 << endl;
  //cout << "Phase 6 start time: " << axis.debug_time_5 / 1000000.0 << endl;
  //cout << "Phase 7 start time: " << axis.debug_time_6 / 1000000.0 << endl;
  //cout << "End time: " << axis.debug_time_7 / 1000000.0 << endl;
  //cout << "Phase 3 delay: " << axis.phase_3_delay << endl;

  vector<double> beginingAndEndTime;
  beginingAndEndTime.push_back(*t.begin());
  beginingAndEndTime.push_back(t.back());
 
  // Prepare the window 
  plt::figure_size(1200, 780);

  // Plot position
  plt::subplot(2,2,1);
  plt::title("Position");
  //plt::plot(s, p, "r-");
  plt::plot(t, p, "r-");

  vector<double> startPosition;
  startPosition.push_back(0);
  startPosition.push_back(0);

  vector<double> destinationPosition;
  destinationPosition.push_back(destination);
  destinationPosition.push_back(destination);

  plt::plot(beginingAndEndTime, startPosition, "k-");
  plt::plot(beginingAndEndTime, destinationPosition, "k-");
  
  // Plot delay 
  plt::subplot(2,2,2);
  plt::title("Delay");
  plt::plot(t, d, "k-");
  //plt::plot(s, d, "k-");
  
  //// Plot delay_p
  //plt::subplot(2,2,3);
  //plt::title("Delay_p");
  //plt::plot(t, d_p, "b-");
  ////plt::plot(s, d_p, "b-");

  // Set x-axis to interval [0,1000000]
  //plt::xlim(0, 1000*1000);

  // Add graph title
  // Enable legend.
  //plt::legend();

  //plt::show(false);
  plt::show();

  //beforeRenderScene();
  //drawLines(t,p,"r-");
  //show();
  //beforeRenderScene();
  //drawLines(t,d,"k-");
  //show();
  //beforeRenderScene();
  //drawLines(t,d_p,"b-");
  //show();

  // So it should take one second to reach the destination
}

int main (int argc, char *argv[]) {

  signal(SIGINT, signalHandler);

  testAcceleration();

}
