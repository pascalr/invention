#include <iostream>
#include <cstring>
#include <math.h>

#include "programme/axis.h"
#include "programme/setup.h"
#include "programme/common.h"

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

template <class T>
void assertTest(T t1, T t2) {
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
  cout << "Testing parseMove" << endl;
  char msg[] = "MX10";
  parseMove(axes, msg);
  assertTest(10.0, AXIS('X')->getPositionUnit());
}

void testNumberLength() {
  cout << "Testing numberLength" << endl;
  assertTest(1, numberLength("8"));
  assertTest(2, numberLength("12"));
  assertTest(3, numberLength("456"));
  cout << "Testing numberLength ignores letter at the beginning" << endl;
  assertTest(3, numberLength("abc456"));
}

void testNextNumber() {
  cout << "Testing nextNumber" << endl;
  assertTest(20.0, nextNumber("MX20"));
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
  testNumberLength();
  testNextNumber();
  testParseMove(axes);

  cout << "All the axes names: " << endl;
  for (int i = 0; i < NB_AXES ; i++) {
    cout << axes[i]->name << endl;
  }
}
