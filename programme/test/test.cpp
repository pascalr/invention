#include <iostream>
#include <cstring>
#include <math.h>

#include "../axis.h"
#include "../setup.h"
#include "../common.h"

using namespace std;

class ConsoleWriter : public Writer {
  public:
    void doPinMode(int pin, bool type) {
      cout << "pinMode(" << pin << ", " << type << ")" << endl;
    }
    void doDigitalWrite(int pin, bool value) {
      cout << "digitalWrite(" << pin << ", " << value << ")" << endl;
    }
    double doDigitalRead(int pin) {
      cout << "digitalRead(" << pin << ")" << endl;
    }
    void doPrint(const char* theString) {
      cout << theString;
    }
    void doPrintLn(const char* theString) {
      cout << theString << endl;
    }
};

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

// Axis by name should be case insensitive
void testAxisByLetter(Axis** axes) {
  cout << "Testing axisByName, should be case insensitive" << endl;
  assertTest('X', axisByLetter(axes, 'x')->name);

  cout << "Testing axisByName, testing macro" << endl;
  assertTest('X', AXIS('X')->name);
}

// MX10 should move axis X 10mm
// MZ269 should move the axis T and the axis X
void testParseMove(Axis** axes) {
  for (int i = 0; i < NB_AXES; i++) {
    axes[i]->referenceReached();
  }
  cout << "Testing parseMove" << endl;
  char msg[] = "MX10";
  parseMove(axes, msg);
  assertTest(msg, 10.0, AXIS('X')->getDestination());
  char msg2[] = "MX20";
  parseMove(axes, msg2);
  assertTest(msg2, 20.0, AXIS('X')->getDestination());
  char msg3[] = "MZ100";
  parseMove(axes, msg3);
  assertTest("MZ100,X", 20.0, AXIS('X')->getDestination());
  assertTest("MZ100,T", 20.0, AXIS('T')->getDestination());
}

void testAtof() {
  cout << "Testing atof, I hope it discards letters after the number" << endl;
  assertTest(20.0, atof("20.0Y10.0"));
}

void testStop(Axis* axis) {
  cout << "Testing stop" << endl;
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

void testSpeed(Axis* axis) {
  cout << "Testing speed" << endl;
  assertNearby("speed", 10.0, axis->getSpeed());
}

int main (int argc, char *argv[]) {
  cout << "Debugging..." << endl;

  Writer* writer = new ConsoleWriter();
  HorizontalAxis* axisX = new HorizontalAxis(writer, 'X');
  VerticalAxis* axisY = new VerticalAxis(writer, 'Y');
  Axis* axisT = new Axis(writer, 'T');
  setupAxes(writer, axisX, axisY, axisT);

  Axis* axes[] = {axisX, axisY, axisT};
  
  testAxisByLetter(axes);
  testParseMove(axes);
  testAtof();
  testStop(axisX);
  testSpeed(axisT);
}
