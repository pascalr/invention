#include <thread>
#include <chrono>
#include <string>
#include <cstring>

#include "fake_program.h"
#include "input_parser.h"

using namespace std;

/*void FakeProgram::execute(const char* dest) {

  string str = dest;
  str += '\n';
  m_reader.setFakeInput(str);

  setCurrentTime(0);
  myLoop(*this);
  while (isWorking) {
    setCurrentTime(currentTime + 5);
    myLoop(*this);
  }

}*/

void FakeProgram::sleepMs(int time) {
  this_thread::sleep_for(chrono::milliseconds(time));
}

/*void FakeProgram::move(char axis, double destination) {
  string str0 = "M";
  str0 += axis;
  str0 += to_string(destination);
  execute(str0.c_str());
}*/
