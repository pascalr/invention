#include <iostream>

using namespace std;

//class ConsoleWriter : public Writer {
class ConsoleWriter {
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
    void doPrint(string theString) {
      cout << "Output sentence";
    }
    void doPrintLn(string theString) {
      cout << "Output sentence";
    }
};

int main (int argc, char *argv[]) {
  //Axis* axis;
  char name = 'A':
  std::cout << "Debugging..." << ""+A << endl;
}
