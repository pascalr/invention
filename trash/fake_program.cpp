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
