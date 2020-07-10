#include <thread>
#include <chrono>
#include <string>
#include <cstring>

#include "fake_program.h"

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
