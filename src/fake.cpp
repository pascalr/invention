#include <iostream>
#include <cstring>
#include <math.h>

#include "programme/axis.h"
#include "programme/setup.h"
#include "programme/common.h"

#include "programme/test/matplotlibcpp.h" // FIXME
      
#include <chrono> // for sleep
#include <thread> // for sleep

#include <signal.h>

#include "programme/test/console_writer.h"

using namespace std;
namespace plt = matplotlibcpp;

#define MESSAGE_RECEIVED "ok"
#define MESSAGE_DONE "done"

void signalHandler( int signum ) {
   cout << "Interrupt signal (" << signum << ") received.\n";

   exit(signum);
}

void show(Axis** axes) {
  HorizontalAxis* axisX = (HorizontalAxis*)axisByLetter(axes, 'X');
  Axis* axisZ = axisByLetter(axes, 'Z');
  
  plt::clf();
  
  vector<double> x(1);
  vector<double> z(1);
  
  x[0] = axisX->getPosition();
  z[0] = axisZ->getPosition();
  plt::plot(x,z,"ro");

  x.push_back(axisX->getPosition() - axisX->getDeltaPosition());
  z.push_back(0.0);
  plt::plot(x,z,"b-");

  plt::xlim(0.0, axisX->getMaxPosition());
  plt::ylim(0.0, axisZ->getMaxPosition());

  plt::title("Position du bras");
  plt::pause(0.01);
}

int main (int argc, char *argv[]) {

  signal(SIGINT, signalHandler);

  ConsoleWriter writer = ConsoleWriter();
  VerticalAxis axisY = VerticalAxis(writer, 'Y');
  HorizontalAxis axisX = HorizontalAxis(writer, 'X');
  Axis axisA = Axis(writer, 'A');
  Axis axisB = Axis(writer, 'B');
  ZAxis axisZ = ZAxis(writer, 'Z', &axisX);
  Axis axisT = Axis(writer, 'T');

  Axis* axes[] = {&axisX, &axisY, &axisT, &axisA, &axisB, &axisZ, NULL};
  setupAxes(&writer, axes);

  while (true) {

    show(axes);

    cerr << ">> ";
    string cmd;
    cin >> cmd;
    cerr << "RECEIVED SOMETHING!" << endl;

    unsigned long currentTime = 0;

    for (int i = 0; axes[i] != NULL; i++) {
      axes[i]->prepare(currentTime);
    }

    parseInput(cmd.c_str(), &writer, axes, 0);

    for (int i = 0; axes[i] != NULL; i++) {
      axes[i]->afterInput();
    }

    bool stillWorking = true;
    while (stillWorking) {
      stillWorking = false;
      for (int i = 0; axes[i] != NULL; i++) {
        stillWorking = stillWorking || axes[i]->handleAxis(currentTime);
      }
      if (currentTime % 200000 == 0 || !stillWorking) {
        show(axes);
      }
      currentTime++;
    }
  }

  return 0;
}
