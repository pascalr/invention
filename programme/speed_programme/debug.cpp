#include <iostream>

#include "programme.ino"

using namespace std;

class ConsoleWriter : public Writer {
  public:
    void doPinMode(int pin, bool type) {
      cout << "pinMode(";
      cout << pin;
      cout << ", ";
      cout << type;
    }
    void doDigitalWrite(int pin, bool value) {
      cout << "Output sentence";
    }
    double doDigitalRead(int pin) {
      cout << "Output sentence";
    }
    void doPrint(String theString) {
      cout << "Output sentence";
    }
    void doPrintLn(String theString) {
      cout << "Output sentence";
    }
};

int main (int argc, char *argv[]) {
  Axis* axis;
  std::cout << "Debugging..." << endl;
}
