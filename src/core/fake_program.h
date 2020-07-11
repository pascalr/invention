#ifndef FAKE_PROGRAM_H
#define FAKE_PROGRAM_H

#include "program.h"
#include "console_writer.h"

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

    void sleepMs(int time);

    int getByte();

    bool getInput(char* buf, int size);

    void setFakeInput(const char* str);

    void setFakeInput(std::string& str);

    bool inputAvailable();

    void execute(const char* dest);

    void move(char axis, double destination);

  protected:
    ConsoleWriter m_writer;
    unsigned long currentTime = 0;
    std::string fake_input;
};


#endif
