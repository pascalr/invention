#ifndef FAKE_PROGRAM_H
#define FAKE_PROGRAM_H

#include "program.h"
#include "reader/fake_reader.h"
#include "writer/console_writer.h"

class FakeProgram : public Program {
  public:
    FakeProgram() : Program(m_writer, m_reader) {
    }

    Writer& getWriter() {
      return m_writer;
    }

    Reader& getReader() {
      return m_reader;
    } 

    void setFakeInput(const char* str) {
      m_reader.setFakeInput(str);
    }
    
    void setFakeInput(std::string& str) {
      m_reader.setFakeInput(str);
    }

    void sleepMs(int time);

    void setCurrentTime(unsigned long time) {
      currentTime = time;
    }

    void execute(const char* dest);

    void move(char axis, double destination);

    unsigned long currentTime = 0;

    unsigned long getCurrentTime() {
      return currentTime;
    }

  protected:

    FakeReader m_reader;
    ConsoleWriter m_writer;
};


#endif
