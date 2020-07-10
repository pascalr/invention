#ifndef IO_PROGRAM_H
#define IO_PROGRAM_H

#include "../utils/utils.h"
#include "../utils/io_common.h"
#include "program.h"
#include <thread>
#include <chrono>
#include "console_writer.h"

using namespace std;

class IOProgram : public Program {
  public:
    IOProgram() : Program(m_writer) {
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
      char c;
      cin.get(c);
      return c;
    }

    bool getInput(char* buf, int size) {
      string str;
      cin >> str;

      if (str.length() >= size-1) {
        cerr << "Message is too long. Maximum: " << size << ". Was: " << str.length() << ".\n";
        return false;
      }

      strcpy(buf, str.c_str());
      rtrim(buf);
      return true;
    }

    bool inputAvailable() {
      return linuxInputAvailable();
    }

  protected:
    ConsoleWriter m_writer;
    unsigned long currentTime = 0;
};


#endif
