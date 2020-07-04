#include <iostream>
#include <cstring>
#include <math.h>

#include "../axis.h"
#include "../setup.h"
#include "../common.h"

#include "matplotlibcpp.h"
      
#include <chrono> // for sleep
#include <thread> // for sleep

#include "console_writer.h"

#include <signal.h>

#include "../../fake_program.h"

// FIXME: MX10.0

using namespace std;
namespace plt = matplotlibcpp;

FakeProgram p;

/*class PlotWriter : public ConsoleWriter {
  public:
    PlotWriter(Axis** axes) : m_axes(axes) {
    }

    void doDigitalWrite(int pin, bool value) {
      ConsoleWriter::doDigitalWrite(pin,value);
    }
  protected:
    Axis** m_axes;
    vector<int> positions;
};*/

template <class P>
void assertNearby(const char* info, P t1, P t2) {
  cout << ((t1 - t2) < 0.5 && (t2 - t1) < 0.5 ? "\033[32mPASSED\033[0m" : "\033[31mFAILED\033[0m");
  cout << " (" << info << ")";
  cout << " - Expected: " << t1 << ", " << "Got: " << t2 << endl;
}

template <class P>
void assertTest(const char* info, P t1, P t2) {
  cout << (t1 == t2 ? "\033[32mPASSED\033[0m" : "\033[31mFAILED\033[0m");
  cout << " (" << info << ")";
  cout << " - Expected: " << t1 << ", " << "Got: " << t2 << endl;
}

template <class P>
void assertTest(P t1, P t2) {
  cout << (t1 == t2 ? "\033[32mPASSED\033[0m" : "\033[31mFAILED\033[0m");
  cout << " - Expected: " << t1 << ", " << "Got: " << t2 << endl;
}

void title(const char* str) {
  cout << "\033[36;1m" << str << "\033[0m" << endl;
}

void move(const char* dest, Writer* writer, Axis** axes, bool plot=false) {
  unsigned long currentTime = 0;

  cout << "Move: " << dest << endl;

  string str = dest;
  p.setFakeInput(str);

  p.setCurrentTime(0);
  myLoop(p);
  while (p.isWorking) {
    p.setCurrentTime(p.getCurrentTime() + 5);
    myLoop(p);
  }

  /*for (int i = 0; axes[i] != NULL; i++) {
    axes[i]->prepare(currentTime);
  }

  parseInput(dest, writer, axes, 0);

  for (int i = 0; axes[i] != NULL; i++) {
    axes[i]->afterInput();
  }

  bool stillWorking = true;
  while (stillWorking) {
    stillWorking = false;
    for (int i = 0; axes[i] != NULL; i++) {
      stillWorking = stillWorking || axes[i]->handleAxis(currentTime);
    }
    if (plot && currentTime % 200000 == 0 || !stillWorking) {

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
    currentTime++;
  }

  if (plot) {
    this_thread::sleep_for(chrono::milliseconds(500));
  }*/
  
}

void move(char axis, double destination, Writer* writer, Axis** axes, bool plot=false) {
  string str0 = "M";
  str0 += axis;
  str0 += to_string((int)destination); // FIXME
  move(str0.c_str(), writer, axes, plot);
}

void referenceAll(Axis** axes) {
  for (int i = 0; axes[i] != NULL; i++) {
    axes[i]->referenceReached();
  }
}

// Axis by name should be case insensitive
void testAxisByLetter(Axis** axes) {
  title("Testing axisByName");

  assertTest("Should be case insensitive", 'X', axisByLetter(axes, 'x')->name);

  cout << "Testing axisByName" << endl;
  assertTest("testing macro", 'X', AXIS('X')->name);
}

// MX10 should move axis X 10mm
// MZ269 should move the axis T and the axis X
void testParseMove(Axis** axes) {
  title("Testing parseMove");

  int cursor;

  for (int i = 0; axes[i] != NULL; i++) {
    axes[i]->referenceReached();
  }

  char msg[] = "X10";
  cursor = parseMove(axes, msg, 0);
  assertTest(msg, 10.0, AXIS('X')->getDestination());
  assertTest("Cursor should go forward", 3, cursor);
  char msg2[] = "Y20X20";
  cursor = parseMove(axes, msg2, 0);
  assertTest(msg2, 20.0, AXIS('X')->getDestination());
  assertTest(msg2, 20.0, AXIS('Y')->getDestination());
  assertTest("Cursor should go forward", 6, cursor);
  //char msg3[] = "Z100";
  //cursor = parseMove(axes, msg3, 0);
  //assertTest("Z100,X", 20.0, AXIS('X')->getDestination());
}

void testStop(Axis* axis) {
  title("Testing stop");

  axis->referenceReached();
  axis->setPositionSteps(10.0*axis->stepsPerUnit);
  axis->setDestination(20.0);
  axis->stop();
  assertNearby("position", 10.0, axis->getPosition());
  assertNearby("destination", 10.0, axis->getDestination());
  axis->forceRotation = true;
  axis->stop();
  assertTest(false, axis->forceRotation);
}

void testParseInput(Writer* writer, Axis** axes) {
  title("Testing parseInput");

  Axis* axisX = axisByLetter(axes, 'X');
  Axis* axisY = axisByLetter(axes, 'Y');
  axisX->setPositionSteps(100.0);
  axisY->setPositionSteps(100.0);
  int cursor;

  cursor = parseInput("HX", writer, axes, 0);
  assertTest("HX should reference X", 0.0, axisX->getPositionSteps());
  assertTest("HX should not refence Y", 100.0, axisY->getPositionSteps());
  assertTest("Should increase the cursor by 2", 2, cursor);
  
  cursor = parseInput("HHX", writer, axes, 1);
  assertTest("HHX index 1 should not refence Y", 100.0, axisY->getPositionSteps());

  cursor = parseInput("H", writer, axes, 0);
  assertTest("H should reference Y", 0.0, axisY->getPositionSteps());
  assertTest("Should increase the cursor by 1", 1, cursor);

}

void testHandleAxis(Writer* writer, Axis** axes) {
  title("Testing handleAxis");

  Axis* axis = axisByLetter(axes, 'X');
  axis->referenceReached();
  unsigned long time_us = 100;

  assertTest("Should not be working at first", false, axis->handleAxis(time_us));

  parseMove(axes, "X10", 0);
  assertTest("After a move it should work", true, axis->handleAxis(time_us));
}

void debug() {}


void testMoveZ(Writer* writer, Axis** axes) {
  title("Testing move Z");

  Axis* axisZ = axisByLetter(axes, 'Z');

  referenceAll(axes);
  
  assertNearby("Beginning position Z", 0.0, axisZ->getPosition());
  assertNearby("Beginning destination Z", 0.0, axisZ->getDestination());

  move('Z', RAYON, writer, axes);
  
  assertNearby("MZ(RAYON) position Z", RAYON, axisZ->getPosition());
  assertNearby("MZ(RAYON) destination Z", RAYON, axisZ->getDestination());
}

void testMoveSquare(Writer* writer, Axis** axes) {
  title("Testing move square");

  HorizontalAxis* axisX = (HorizontalAxis*)axisByLetter(axes, 'X');
  Axis* axisZ = axisByLetter(axes, 'Z');

  referenceAll(axes);

  // (X,Z)
  // (RAYON,0) ->
  // (RAYON,RAYON) ->
  // (0,RAYON) ->
  // (0,0) ->
  // (RAYON,0)

  // BEGINNING
  assertNearby("Beginning position X", RAYON, axisX->getPosition());
  assertNearby("Beginning destination X", RAYON, axisX->getDestination());
  assertNearby("Beginning delta X", RAYON, axisX->getDeltaPosition());
  assertNearby("Beginning position Z", 0.0, axisZ->getPosition());
  assertNearby("Beginning destination Z", 0.0, axisZ->getDestination());

  move('Z', RAYON, writer, axes, true);
  
  assertNearby("MZ380 position X", RAYON, axisX->getPosition());
  assertNearby("MZ380 destination X", RAYON, axisX->getDestination());
  assertNearby("MZ380 delta X", 0.0, axisX->getDeltaPosition());
  assertNearby("MZ380 position Z", RAYON, axisZ->getPosition());
  assertNearby("MZ380 destination Z", RAYON, axisZ->getDestination());
  
  move("MX0", writer, axes, true);
  
  assertNearby("MX0 position X", 0.0, axisX->getPosition());
  assertNearby("MX0 destination X", 0.0, axisX->getDestination());
  assertNearby("MX0 delta X", 0.0, axisX->getDeltaPosition());
  assertNearby("MX0 position Z", RAYON, axisZ->getPosition());
  assertNearby("MX0 destination Z", RAYON, axisZ->getDestination());

  move("MZ0", writer, axes, true);
  
  assertNearby("MZ0 position X", 0.0, axisX->getPosition());
  assertNearby("MZ0 destination X", 0.0, axisX->getDestination());
  assertNearby("MZ0 delta X", -RAYON, axisX->getDeltaPosition());
  assertNearby("MZ0 position Z", 0.0, axisZ->getPosition());
  assertNearby("MZ0 destination Z", 0.0, axisZ->getDestination());

  move('X', RAYON, writer, axes, true);

  assertNearby("MX380 position X", RAYON, axisX->getPosition());
  assertNearby("MX380 destination X", RAYON, axisX->getDestination());
  assertNearby("MX380 delta X", RAYON, axisX->getDeltaPosition());
  assertNearby("MX380 position Z", 0.0, axisZ->getPosition());
  assertNearby("MX380 destination Z", 0.0, axisZ->getDestination());
}

void testMoveXFlipsZ(Writer* writer, Axis** axes) {
  title("Testing move x flips z");

  HorizontalAxis* axisX = (HorizontalAxis*)axisByLetter(axes, 'X');
  ZAxis* axisZ = (ZAxis*)axisByLetter(axes, 'Z');

  referenceAll(axes);

  assertNearby("Beginning position X", RAYON, axisX->getPosition());
  assertNearby("Beginning destination X", RAYON, axisX->getDestination());
  assertNearby("Beginning delta X", RAYON, axisX->getDeltaPosition());
  assertNearby("Beginning position Z", 0.0, axisZ->getPosition());
  assertNearby("Beginning destination Z", 0.0, axisZ->getDestination());
  assertNearby("Beginning angle Z", 0.0, axisZ->getDestinationAngle());

  move("MX0", writer, axes, true);
  
  assertNearby("MX0 position X", 0.0, axisX->getPosition());
  assertNearby("MX0 destination X", 0.0, axisX->getDestination());
  assertNearby("MX0 delta X", -RAYON, axisX->getDeltaPosition());
  assertNearby("MX0 position Z", 0.0, axisZ->getPosition());
  assertNearby("MX0 angle Z", 180.0, axisZ->getDestinationAngle());

  move('X', AXIS_X_MAX_POS, writer, axes, true);
  
  assertNearby("MX_max position X", AXIS_X_MAX_POS, axisX->getPosition());
  assertNearby("MX_max destination X", AXIS_X_MAX_POS, axisX->getDestination());
  assertNearby("MX_max delta X", RAYON, axisX->getDeltaPosition());
  assertNearby("MX_max position Z", 0.0, axisZ->getPosition());
  assertNearby("MX_max angle Z", 0.0, axisZ->getDestinationAngle());
}

void testMoveZMovesX(Writer* writer, Axis** axes) {
  title("Testing move z moves x");

  HorizontalAxis* axisX = (HorizontalAxis*)axisByLetter(axes, 'X');

  referenceAll(axes);

  // BEGINNING
  assertNearby("Beginning position X", RAYON, axisX->getPosition());
  assertNearby("Beginning destination X", RAYON, axisX->getDestination());
  assertNearby("Beginning delta X", RAYON, axisX->getDeltaPosition());

  move("MX100", writer, axes);
  
  assertNearby("MX100 position X", 100.0, axisX->getPosition());
  assertNearby("MX100 destination X", 100.0, axisX->getDestination());
  assertNearby("MX100 delta X", -RAYON, axisX->getDeltaPosition());
  
  referenceAll(axes);
  move('Z',RAYON, writer, axes);
  
  assertNearby("MZ380 position X", RAYON, axisX->getPosition());
  assertNearby("MZ380 destination X", RAYON, axisX->getDestination());
  assertNearby("MZ380 delta X", 0.0, axisX->getDeltaPosition());

  move("MX0", writer, axes);
  
  assertNearby("MX0 position X", 0.0, axisX->getPosition());
  assertNearby("MX0 destination X", 0.0, axisX->getDestination());
  assertNearby("MX0 delta X", 0.0, axisX->getDeltaPosition());

  // axisX->getDestination() -> tip destination

  // Tip starts at X380, and delta is 380
  // Base position is dest - delta
  // By moving Z380, the tip stays at 380
  // But the delta becomes 0.
  /*parseInput("MZ380", writer, axes, 0);
  assertNearby("Position X is rayon first", RAYON, axisX->getPosition());
  steps = axisZ->getDestinationSteps();
  for (int i = 0; i < steps; i++) {
    axisZ->turnOneStep();
  }

  assertNearby("Destination X", RAYON, axisX->getDestination());
  assertTest("Destination steps X", RAYON * axisX->getStepsPerUnit(), axisX->getDestinationSteps());

  assertNearby("Delta position X", 0.0, axisX->getDeltaPosition());
  steps = axisX->getDestinationSteps();
  for (int i = 0; i < steps; i++) {
    axisX->turnOneStep();
  }
  assertNearby("Position X", 380.0, axisX->getPosition());

  parseInput("MZ0", writer, axes, 0);
  assertNearby("Destination X", RAYON, axisX->getDestination());
  for (int i = 0; i < 100000 && !axisZ->isDestinationReached(); i++) {
    debug();
    axisZ->turnOneStep();
  }
  assertNearby("Destination X", RAYON, axisX->getDestination());*/

}

void signalHandler( int signum ) {
   cout << "Interrupt signal (" << signum << ") received.\n";

   exit(signum);
}

int main (int argc, char *argv[]) {

  signal(SIGINT, signalHandler);

  setupAxes(&p.getWriter(), p.axes);

  //plt::figure_size(axisX.getMaxPosition(), axisZ.getMaxPosition());
  
  //plt::ion();

  testAxisByLetter(p.axes);
  testParseMove(p.axes);
  testStop(axisByLetter(p.axes,'Y'));
  testParseInput(&p.getWriter(), p.axes);
  testHandleAxis(&p.getWriter(), p.axes);
  testMoveZ(&p.getWriter(), p.axes);
  testMoveZMovesX(&p.getWriter(), p.axes);
  testMoveSquare(&p.getWriter(), p.axes);
  testMoveXFlipsZ(&p.getWriter(), p.axes);

}
