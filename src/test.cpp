#include <iostream>
#include <cstring>
#include <math.h>

#include "lib/lib.h"
#include "lib/matplotlibcpp.h"
#include "core/console_writer.h"
#include "core/axis.h"
#include "core/input_parser.h"
#include "config/setup.h"
      
#include <chrono> // for sleep
#include <thread> // for sleep

#include <signal.h>

#include "core/serialize.h"
#include "core/deserialize.h"
#include "core/fake_program.h"
#include "core/string_writer.h"

using namespace std;
namespace plt = matplotlibcpp;


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

template <class P1, class P2>
void assertNearby(const char* info, P1 t1, P2 t2) {
  cout << ((t1 - t2) < 0.5 && (t2 - t1) < 0.5 ? "\033[32mPASSED\033[0m" : "\033[31mFAILED\033[0m");
  cout << " (" << info << ")";
  cout << " - Expected: " << t1 << ", " << "Got: " << t2 << endl;
}

template <class P1, class P2>
void assertTest(const char* info, P1 t1, P2 t2) {
  cout << (t1 == t2 ? "\033[32mPASSED\033[0m" : "\033[31mFAILED\033[0m");
  cout << " (" << info << ")";
  cout << " - Expected: " << t1 << ", " << "Got: " << t2 << endl;
}

template <class P1, class P2>
void assertTest(P1 t1, P2 t2) {
  cout << (t1 == t2 ? "\033[32mPASSED\033[0m" : "\033[31mFAILED\033[0m");
  cout << " - Expected: " << t1 << ", " << "Got: " << t2 << endl;
}

void title(const char* str) {
  cout << "\033[36;1m" << str << "\033[0m" << endl;
}

void move(const char* dest, FakeProgram& p, bool plot=false) {
  unsigned long currentTime = 0;

  cout << "Move: " << dest << endl;

  string str = dest;
  str += '\n';
  p.setFakeInput(str);

  p.setCurrentTime(0);
  myLoop(p);
  while (p.isWorking) {
    p.setCurrentTime(p.getCurrentTime() + 5);
    myLoop(p);
  }

}

void move(char axis, double destination, FakeProgram& p, bool plot=false) {
  string str0 = "M";
  str0 += axis;
  str0 += to_string((int)destination); // FIXME
  move(str0.c_str(), p, plot);
}

void referenceAll(Axis** axes) {
  for (int i = 0; axes[i] != NULL; i++) {
    axes[i]->referenceReached();
  }
}

void testStop() {
  title("Testing stop");

  FakeProgram p;
  setupAxes(p);

  Axis* axis = &p.axisY;
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

void testParseInput() {
  title("Testing parseInput");

  FakeProgram p;
  setupAxes(p);

  p.axisZ.setPositionSteps(100);
  p.axisY.setPositionSteps(100);

  /*cursor = parseInput("HX", p, 0);
  assertTest("HX should reference X", 0, axisX.getPositionSteps());
  assertTest("HX should not refence Y", 100, axisY->getPositionSteps());
  assertTest("Should increase the cursor by 2", 2, cursor);*/

  p.setFakeInput("H");
  myLoop(p);
  assertTest("H should reference Z", 0, p.axisZ.getPositionSteps());
  assertTest("H should reference Y", 0, p.axisY.getPositionSteps());

}

void testMoveZ() {
  title("Testing move Z");

  FakeProgram p;
  setupAxes(p);

  referenceAll(p.axes);
  
  assertNearby("Beginning position Z", 0.0, p.axisZ.getPosition());
  assertNearby("Beginning destination Z", 0.0, p.axisZ.getDestination());

  move('Z', RAYON, p);
  
  assertNearby("MZ(RAYON) position Z", RAYON, p.axisZ.getPosition());
  assertNearby("MZ(RAYON) destination Z", RAYON, p.axisZ.getDestination());
}

void testMoveSquare() {
  title("Testing move square");

  FakeProgram p;
  setupAxes(p);

  referenceAll(p.axes);

  // (X,Z)
  // (RAYON,0) ->
  // (RAYON,RAYON) ->
  // (0,RAYON) ->
  // (0,0) ->
  // (RAYON,0)

  // BEGINNING
  assertNearby("Beginning position X", RAYON, p.axisX.getPosition());
  assertNearby("Beginning destination X", RAYON, p.axisX.getDestination());
  assertNearby("Beginning delta X", RAYON, p.axisX.getDeltaPosition());
  assertNearby("Beginning position Z", 0.0, p.axisZ.getPosition());
  assertNearby("Beginning destination Z", 0.0, p.axisZ.getDestination());

  move('Z', RAYON, p, true);
  
  assertNearby("MZ380 position X", RAYON, p.axisX.getPosition());
  assertNearby("MZ380 destination X", RAYON, p.axisX.getDestination());
  assertNearby("MZ380 delta X", 0.0, p.axisX.getDeltaPosition());
  assertNearby("MZ380 position Z", RAYON, p.axisZ.getPosition());
  assertNearby("MZ380 destination Z", RAYON, p.axisZ.getDestination());
  
  move("MX0", p, true);
  
  assertNearby("MX0 position X", 0.0, p.axisX.getPosition());
  assertNearby("MX0 destination X", 0.0, p.axisX.getDestination());
  assertNearby("MX0 delta X", 0.0, p.axisX.getDeltaPosition());
  assertNearby("MX0 position Z", RAYON, p.axisZ.getPosition());
  assertNearby("MX0 destination Z", RAYON, p.axisZ.getDestination());

  move("MZ0", p, true);
  
  assertNearby("MZ0 position X", 0.0, p.axisX.getPosition());
  assertNearby("MZ0 destination X", 0.0, p.axisX.getDestination());
  assertNearby("MZ0 delta X", -RAYON, p.axisX.getDeltaPosition());
  assertNearby("MZ0 position Z", 0.0, p.axisZ.getPosition());
  assertNearby("MZ0 destination Z", 0.0, p.axisZ.getDestination());

  move('X', RAYON, p, true);

  assertNearby("MX380 position X", RAYON, p.axisX.getPosition());
  assertNearby("MX380 destination X", RAYON, p.axisX.getDestination());
  assertNearby("MX380 delta X", RAYON, p.axisX.getDeltaPosition());
  assertNearby("MX380 position Z", 0.0, p.axisZ.getPosition());
  assertNearby("MX380 destination Z", 0.0, p.axisZ.getDestination());
}

void testMoveXFlipsZ() {
  title("Testing move x flips z");

  FakeProgram p;
  setupAxes(p);

  referenceAll(p.axes);

  assertNearby("Beginning position X", RAYON, p.axisX.getPosition());
  assertNearby("Beginning destination X", RAYON, p.axisX.getDestination());
  assertNearby("Beginning delta X", RAYON, p.axisX.getDeltaPosition());
  assertNearby("Beginning position Z", 0.0, p.axisZ.getPosition());
  assertNearby("Beginning destination Z", 0.0, p.axisZ.getDestination());
  assertNearby("Beginning angle Z", 0.0, p.axisZ.getDestinationAngle());

  move("MX0", p, true);
  
  assertNearby("MX0 position X", 0.0, p.axisX.getPosition());
  assertNearby("MX0 destination X", 0.0, p.axisX.getDestination());
  assertNearby("MX0 delta X", -RAYON, p.axisX.getDeltaPosition());
  assertNearby("MX0 position Z", 0.0, p.axisZ.getPosition());
  assertNearby("MX0 angle Z", 180.0, p.axisZ.getDestinationAngle());

  move('X', AXIS_X_MAX_POS, p, true);
  
  assertNearby("MX_max position X", AXIS_X_MAX_POS, p.axisX.getPosition());
  assertNearby("MX_max destination X", AXIS_X_MAX_POS, p.axisX.getDestination());
  assertNearby("MX_max delta X", RAYON, p.axisX.getDeltaPosition());
  assertNearby("MX_max position Z", 0.0, p.axisZ.getPosition());
  assertNearby("MX_max angle Z", 0.0, p.axisZ.getDestinationAngle());
}

void testMoveZMovesX() {
  title("Testing move z moves x");

  FakeProgram p;
  setupAxes(p);

  referenceAll(p.axes);

  // BEGINNING
  assertNearby("Beginning position X", RAYON, p.axisX.getPosition());
  assertNearby("Beginning destination X", RAYON, p.axisX.getDestination());
  assertNearby("Beginning delta X", RAYON, p.axisX.getDeltaPosition());

  move("MX100", p);
  
  assertNearby("MX100 position X", 100.0, p.axisX.getPosition());
  assertNearby("MX100 destination X", 100.0, p.axisX.getDestination());
  assertNearby("MX100 delta X", -RAYON, p.axisX.getDeltaPosition());
  
  referenceAll(p.axes);
  move('Z',RAYON, p);
  
  assertNearby("MZ380 position X", RAYON, p.axisX.getPosition());
  assertNearby("MZ380 destination X", RAYON, p.axisX.getDestination());
  assertNearby("MZ380 delta X", 0.0, p.axisX.getDeltaPosition());

  move("MX0", p);
  
  assertNearby("MX0 position X", 0.0, p.axisX.getPosition());
  assertNearby("MX0 destination X", 0.0, p.axisX.getDestination());
  assertNearby("MX0 delta X", 0.0, p.axisX.getDeltaPosition());

}

void testSerialize() {
  title("Testing serialize");

  FakeProgram prog;
  setupAxes(prog);
  referenceAll(prog.axes);
  
  prog.axisZ.setIsReferenced(true);
  prog.axisZ.setIsReferencing(true);
  prog.axisZ.isForward = true;
  prog.axisZ.setDestination(RAYON);

  stringstream ss;
  serialize<ostream>(prog, ss);

  FakeProgram result;
  
  deserialize(result, ss.str());

  assertNearby("dest", RAYON, result.axisZ.getDestination());
  assertNearby("pos", 0.0, result.axisZ.getPosition());
  assertTest("isReferenced", true, result.axisZ.isReferenced);
  assertTest("isReferencing", true, result.axisZ.isReferencing);
  assertTest("forward", true, result.axisZ.isForward);
  assertNearby("dest_angle", 90.0, result.axisZ.getDestinationAngle());
  assertNearby("angle", 0.0, result.axisZ.getPositionAngle());
}

void testInputParserParseNumber() {
  title("Testing input_parser parseNumber");
  FakeProgram p;
  double val;

  p.setFakeInput("m123\n");
  assertTest("invalid number (m123)", -1, parseNumber(p,val));

  p.setFakeInput("123\n");
  parseNumber(p, val);
  assertNearby("123", 123.0, val);

  p.setFakeInput("-123\n");
  parseNumber(p, val);
  assertNearby("-123", -123.0, val);
  
  p.setFakeInput("+123\n");
  parseNumber(p, val);
  assertNearby("+123", 123.0, val);

  p.setFakeInput("12+3\n");
  parseNumber(p, val);
  assertNearby("12+3", 12.0, val);

  p.setFakeInput("12-3\n");
  parseNumber(p, val);
  assertNearby("12-3", 12.0, val);
  
  p.setFakeInput("123.45\n");
  parseNumber(p, val);
  assertNearby("123.45", 123.45, val);

  p.setFakeInput("\n");
  assertTest("invalid number (empty)", -1, parseNumber(p,val));
}

int main (int argc, char *argv[]) {

  signal(SIGINT, signalHandler);

  //plt::figure_size(axisX.getMaxPosition(), axisZ.getMaxPosition());
  
  //plt::ion();

  testMoveZMovesX();
  testMoveZ();
  testInputParserParseNumber();
  testSerialize();
  testStop();
  testParseInput();
  testMoveXFlipsZ();
  testMoveSquare();

}
