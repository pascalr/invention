#include <thread>
#include <chrono>
#include <string>
#include <cstring>

#include "fake_program.h"
#include "input_parser.h"

using namespace std;

void FakeProgram::sleepMs(int time) {
  this_thread::sleep_for(chrono::milliseconds(time));
}
    
int FakeProgram::getByte() {
  if (fake_input.empty()) {
    return -1;
  }
  int val = fake_input[0];
  fake_input.erase(fake_input.begin());
  return val;
}

bool FakeProgram::getInput(char* buf, int size) {
  if (fake_input.empty()) {
    return false;
  }
  strcpy(buf, fake_input.c_str());
  fake_input.clear();
  return true;
}

void FakeProgram::setFakeInput(const char* str) {
  fake_input.assign(str);
}

void FakeProgram::setFakeInput(std::string& str) {
  fake_input.assign(str);
}

bool FakeProgram::inputAvailable() {
  return !fake_input.empty();
}

void FakeProgram::execute(const char* dest) {
  unsigned long currentTime = 0;

  // cout << "Move: " << dest << endl;

  string str = dest;
  str += '\n';
  setFakeInput(str);

  setCurrentTime(0);
  myLoop(*this);
  while (isWorking) {
    setCurrentTime(getCurrentTime() + 5);
    myLoop(*this);
  }

}

void FakeProgram::move(char axis, double destination) {
  string str0 = "M";
  str0 += axis;
  str0 += to_string(destination);
  execute(str0.c_str());
}
