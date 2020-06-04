#include <iostream>

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
  cout << (t1 == t2 ? "PASSED" : "FAILED") << " - Expected: " << t1 << ", " << "Got: " << t2 << endl;
}

// Axis by name should be case insensitive
void testAxisByLetter(Axis** axes) {
  cout << "Testing axisByName, should be case insensitive" << endl;

  char result = axisByLetter(axes, 'x')->name;

  assertTest('X', result);

  //cout << "Expected: X" << endl << "Got: " << result << endl;
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

  cout << "All the axes names: " << endl;
  for (int i = 0; i < NB_AXES ; i++) {
    cout << axes[i]->name << endl;
  }
}
