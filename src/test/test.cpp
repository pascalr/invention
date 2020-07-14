#include <iostream>
#include <cstring>
#include <math.h>

#include "../lib/lib.h"
#include "../lib/matplotlibcpp.h"
#include "../core/console_writer.h"
#include "../core/axis.h"
#include "../core/input_parser.h"
#include "../config/setup.h"
      
#include <chrono> // for sleep
#include <thread> // for sleep

#include <signal.h>

#include "../core/serialize.h"
#include "../core/deserialize.h"
#include "../core/fake_program.h"

#include "test.h"

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


void referenceAll(MotorAxis** axes) {
  for (int i = 0; axes[i] != NULL; i++) {
    axes[i]->referenceReached();
  }
}

void testStop() {
  title("Testing stop");

  FakeProgram p;
  setupAxes(p);

  p.axisY.referenceReached();
  p.axisY.setPositionSteps(10.0*p.axisY.stepsPerUnit);
  p.axisY.setDestination(20.0);
  p.axisY.stop();
  assertNearby("position", 10.0, p.axisY.getPosition());
  assertNearby("destination", 10.0, p.axisY.getDestination());
  p.axisY.forceRotation = true;
  p.axisY.stop();
  assertTest(false, p.axisY.forceRotation);
}

void testParseInput() {
  title("Testing parseInput");

  FakeProgram p;
  setupAxes(p);

  p.axisA.setPositionSteps(100);
  p.axisY.setPositionSteps(100);

  /*cursor = parseInput("HX", p, 0);
  assertTest("HX should reference X", 0, axisX.getPositionSteps());
  assertTest("HX should not refence Y", 100, axisY->getPositionSteps());
  assertTest("Should increase the cursor by 2", 2, cursor);*/

  p.setFakeInput("H");
  myLoop(p);
  assertTest("H should reference Z", 0, p.axisA.getPositionSteps());
  assertTest("H should reference Y", 0, p.axisY.getPositionSteps());

}

void testMoveZ() {
  title("Testing move Z");

  FakeProgram p;
  setupAxes(p);

  referenceAll(p.motorAxes);
  
  assertNearby("Beginning position Z", 0.0, p.axisZ.getPosition());

  p.move('Z', RAYON);
  
  assertNearby("MZ(RAYON) position Z", RAYON, p.axisZ.getPosition());
  assertNearby("MZ(RAYON) destination Z", RAYON, p.axisZ.getDestination());
}

void testMoveSquare() {
  title("Testing move square");

  FakeProgram p;
  setupAxes(p);

  referenceAll(p.motorAxes);

  // (X,Z)
  // (RAYON,0) ->
  // (RAYON,RAYON) ->
  // (0,RAYON) ->
  // (0,0) ->
  // (RAYON,0)

  // BEGINNING
  assertNearby("Beginning position X", RAYON, p.axisX.getPosition());
  assertNearby("Beginning base X", 0.0, p.baseAxisX.getPosition());
  assertNearby("Beginning position Z", 0.0, p.axisZ.getPosition());

  p.move('Z', RAYON);
  
  assertNearby("MZ380 position X", RAYON, p.axisX.getPosition());
  assertNearby("MZ380 base X", RAYON, p.baseAxisX.getPosition());
  assertNearby("MZ380 position Z", RAYON, p.axisZ.getPosition());
  assertNearby("MZ380 destination Z", RAYON, p.axisZ.getDestination());
  
  p.move('X', 0.0);
  
  assertNearby("MX0 position X", 0.0, p.axisX.getPosition());
  assertNearby("MX0 destination X", 0.0, p.axisX.getDestination());
  assertNearby("MX0 base X", 0.0, p.baseAxisX.getPosition());
  assertNearby("MX0 position Z", RAYON, p.axisZ.getPosition());
  assertNearby("MX0 destination Z", RAYON, p.axisZ.getDestination());

  p.move('Z', 0.0);
  
  assertNearby("MZ0 position X", 0.0, p.axisX.getPosition());
  assertNearby("MZ0 destination X", 0.0, p.axisX.getDestination());
  assertNearby("MZ0 base X", RAYON, p.baseAxisX.getPosition());
  assertNearby("MZ0 position Z", 0.0, p.axisZ.getPosition());
  assertNearby("MZ0 destination Z", 0.0, p.axisZ.getDestination());

  p.move('X', RAYON);

  assertNearby("MX380 position X", RAYON, p.axisX.getPosition());
  assertNearby("MX380 destination X", RAYON, p.axisX.getDestination());
  assertNearby("MX380 axis X", RAYON*2, p.baseAxisX.getPosition());
  assertNearby("MX380 position Z", 0.0, p.axisZ.getPosition());
  assertNearby("MX380 destination Z", 0.0, p.axisZ.getDestination());
}

void debug() {}

void testMoveXFlipsZ() {
  title("Testing move x flips z");

  FakeProgram p;
  setupAxes(p);

  referenceAll(p.motorAxes);

  assertNearby("Beginning position X", RAYON, p.axisX.getPosition());
  assertNearby("Beginning base X", 0.0, p.baseAxisX.getPosition());
  assertNearby("Beginning position Z", 0.0, p.axisZ.getPosition());
  assertNearby("Beginning angle Z", 0.0, p.axisT.getDestination());

  p.move('X', 0.0);
  
  assertNearby("MX0 position X", 0.0, p.axisX.getPosition());
  assertNearby("MX0 destination X", 0.0, p.axisX.getDestination());
  assertNearby("MX0 base X", RAYON, p.baseAxisX.getPosition());
  assertNearby("MX0 position Z", 0.0, p.axisZ.getPosition());
  assertNearby("MX0 angle Z", 180.0, p.axisT.getDestination());

  p.move('X', AXIS_X_MAX_POS);
  
  assertNearby("MX_max position X", AXIS_X_MAX_POS, p.axisX.getPosition());
  assertNearby("MX_max destination X", AXIS_X_MAX_POS, p.axisX.getDestination());
  assertNearby("MX_max base X", AXIS_X_MAX_POS-RAYON, p.baseAxisX.getPosition());
  assertNearby("MX_max position Z", 0.0, p.axisZ.getPosition());
  assertNearby("MX_max angle Z", 0.0, p.axisT.getDestination());
}

void testMoveZMovesX() {
  title("Testing move z moves x");

  FakeProgram p;
  setupAxes(p);

  referenceAll(p.motorAxes);

  // BEGINNING
  assertNearby("Beginning position X", RAYON, p.axisX.getPosition());
  assertNearby("Beginning base X", 0.0, p.baseAxisX.getPosition());

  p.move('X', 100.0);
  
  assertNearby("MX100 position X", 100.0, p.axisX.getPosition());
  assertNearby("MX100 destination X", 100.0, p.axisX.getDestination());
  assertNearby("MX100 base X", 100.0 + RAYON, p.baseAxisX.getPosition());

  debug();
  
  referenceAll(p.motorAxes);
  p.move('Z',RAYON);
  
  assertNearby("MZ380 position X", RAYON, p.axisX.getPosition());
  assertNearby("MZ380 destination X", RAYON, p.axisX.getDestination());
  assertNearby("MZ380 base X", RAYON, p.baseAxisX.getPosition());

  p.move('X', 0.0);
  
  assertNearby("MX0 position X", 0.0, p.axisX.getPosition());
  assertNearby("MX0 destination X", 0.0, p.axisX.getDestination());
  assertNearby("MX0 base X", 0.0, p.baseAxisX.getPosition());

}

void testSerialize() {
  title("Testing serialize");

  FakeProgram prog;
  setupAxes(prog);
  referenceAll(prog.motorAxes);
  
  prog.axisY.setIsReferenced(true);
  prog.axisY.setIsReferencing(true);
  prog.axisY.isForward = true;
  prog.axisY.setDestination(RAYON);

  stringstream ss;
  serialize<ostream>(prog, ss);

  FakeProgram result;
  
  deserialize(result, ss.str());

  assertNearby("dest", RAYON, result.axisY.getDestination());
  //assertNearby("pos", 0.0, result.axisY.getPosition());
  //assertTest("isReferenced", true, result.axisY.isReferenced);
  //assertTest("isReferencing", true, result.axisY.isReferencing);
  //assertTest("forward", true, result.axisY.isForward);
  //assertNearby("dest_angle", 90.0, result.axisY.getDestinationAngle());
  //assertNearby("angle", 0.0, result.axisY.getPositionAngle());
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

  //testSerialize();
  testMoveZMovesX();
  /*testMoveXFlipsZ();
  testStop();
  testMoveZ();
  testParseInput();
  testInputParserParseNumber();
  testMoveSquare();*/

}
