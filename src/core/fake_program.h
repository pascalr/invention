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
    void setCurrentTime(unsigned long time) {
      currentTime = time;
    }

    void sleepMs(int time);

    int getByte();

    void setFakeInput(const char* str);

    void setFakeInput(std::string& str);

    bool inputAvailable();

    void execute(const char* dest);

    void move(char axis, double destination);

    unsigned long currentTime = 0;

    unsigned long getCurrentTime() {
      return currentTime;
    }

  protected:

    ConsoleWriter m_writer;
    std::string fake_input;
};


#endif
