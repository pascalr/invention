#ifndef FAKE_PROGRAM_H
#define FAKE_PROGRAM_H

#include "programme/utils.h"
#include "io_common.h"

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

    bool getInput(char* buf, int size) {
      string str;
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
      rtrim(buf);
      return true;
    }

    void setFakeInput(string& str) {
      fake_input.assign(str);
    }

    bool inputAvailable() {
      if (!fake_input.empty()) {
        return true;
      }
      return linuxInputAvailable();
    }

  protected:
    ConsoleWriter m_writer;
    unsigned long currentTime = 0;
    string fake_input;
};


#endif
