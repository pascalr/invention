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
    void doPrint(char val) {
      cout << val;
    }
    void doPrintLn(const char* theString) {
      cout << theString << endl;
    }
    void doPrintLn(double val) {
      cout << val << endl;
    }
};

class SilentWriter : public Writer {
  public:
    void doPinMode(int pin, bool type) {
    }
    void doDigitalWrite(int pin, bool value) {
    }
    double doDigitalRead(int pin) {
    }
    void doPrint(const char* theString) {
    }
    void doPrint(char val) {
    }
    void doPrintLn(const char* theString) {
    }
    void doPrintLn(double val) {
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
  cout << "Testing axisByName" << endl;
  assertTest("Should be case insensitive", 'X', axisByLetter(axes, 'x')->name);

  cout << "Testing axisByName" << endl;
  assertTest("testing macro", 'X', AXIS('X')->name);
}

// MX10 should move axis X 10mm
// MZ269 should move the axis T and the axis X
void testParseMove(Axis** axes) {
  cout << "Testing parseMove" << endl;
  for (int i = 0; axes[i] != NULL; i++) {
    axes[i]->referenceReached();
  }
  char msg[] = "MX10";
  parseMove(axes, msg, 1);
  assertTest(msg, 10.0, AXIS('X')->getDestination());
  char msg2[] = "MX20";
  parseMove(axes, msg2, 1);
  assertTest(msg2, 20.0, AXIS('X')->getDestination());
  char msg3[] = "MZ100";
  parseMove(axes, msg3, 1);
  assertTest("MZ100,X", 20.0, AXIS('X')->getDestination());
  assertTest("MZ100,T", 20.0, AXIS('T')->getDestination());
}

void testAtof() {
  cout << "Testing atof" << endl;
  assertTest("Should stop parsing at first non number symbol", 20.0, atof("20.0Y10.0"));
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

void testParseInput(Writer* writer, Axis** axes) {
  cout << "Testing parseInput" << endl;

  Axis* axisX = axisByLetter(axes, 'X');
  Axis* axisY = axisByLetter(axes, 'Y');
  axisX->setPositionSteps(100.0);
  axisY->setPositionSteps(100.0);

  int cursor = parseInput("HX", writer, axes, 0);
  assertTest("HX should reference X", 0.0, axisX->getPositionSteps());
  assertTest("HX should not refence Y", 100.0, axisY->getPositionSteps());
  assertTest("Should increase the cursor by 2", 2, cursor);
  
  cursor = parseInput("HHX", writer, axes, 1);
  assertTest("HHX index 1 should not refence Y", 100.0, axisY->getPositionSteps());

  cursor = parseInput("H", writer, axes, 0);
  assertTest("H should reference Y", 0.0, axisY->getPositionSteps());
  assertTest("Should increase the cursor by 1", 1, cursor);

}

int main (int argc, char *argv[]) {
  cout << "Debugging..." << endl;

  SilentWriter writer = SilentWriter();
  HorizontalAxis axisX = HorizontalAxis(&writer, 'X');
  VerticalAxis axisY = VerticalAxis(&writer, 'Y');
  Axis axisT = Axis(&writer, 'T');
  Axis* axes[] = {&axisX, &axisY, &axisT, NULL};
  setupAxes(&writer, axes);
  
  testAxisByLetter(axes);
  testParseMove(axes);
  testAtof();
  testStop(&axisX);
  testSpeed(&axisT);
  testParseInput(&writer, axes);
}
