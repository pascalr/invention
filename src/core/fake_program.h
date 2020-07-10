#ifndef FAKE_PROGRAM_H
#define FAKE_PROGRAM_H

#include "../utils/utils.h"
#include "../utils/io_common.h"
#include "../core/program.h"
#include <thread>
#include <chrono>
#include "console_writer.h"

using namespace std;

class FakeProgram : public Program {
  public:
    FakeProgram() : Program(m_writer) {
    }

    Writer& getWriter() {
      return m_writer;
    }
    unsigned long getCurrentTime() {
      return currentTime;
    }
    void setCurrentTime(unsigned long time) {
      currentTime = time;
    }

    void sleepMs(int time) {
      this_thread::sleep_for(chrono::milliseconds(time));
    }
    
    int getByte() {
      if (fake_input.empty()) {
        return -1;
      }
      int val = fake_input[0];
      fake_input.erase(fake_input.begin());
      return val;
    }

    bool getInput(char* buf, int size) {
      if (fake_input.empty()) {
        return false;
      }
      strcpy(buf, fake_input.c_str());
      fake_input.clear();
      return true;
      /*string str;
      if (fake_input.empty()) {
        cin >> str;
      } else {
        str.assign(fake_input);
        fake_input.clear();
      }

      if (str.length() >= size-1) {
        cerr << "Message is too long. Maximum: " << size << ". Was: " << str.length() << ".\n";
        return false;
      }

      strcpy(buf, str.c_str());
      rtrim(buf);*/
    }

    void setFakeInput(const char* str) {
      fake_input = str;
    }

    void setFakeInput(string& str) {
      fake_input.assign(str);
    }

    bool inputAvailable() {
      return !fake_input.empty();
      /*if (!fake_input.empty()) {
        return true;
      }
      return linuxInputAvailable();*/
    }

  protected:
    ConsoleWriter m_writer;
    unsigned long currentTime = 0;
    string fake_input;
};


#endif
