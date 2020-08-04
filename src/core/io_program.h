#ifndef IO_PROGRAM_H
#define IO_PROGRAM_H

#include "../utils/utils.h"
#include "../utils/io_common.h"
#include "program.h"
#include <thread>
#include <chrono>
#include "console_writer.h"
#include "reader/io_reader.h"

using namespace std;

class IOProgram : public Program {
  public:
    IOProgram() : Program(m_writer, m_reader) {
    }

    Writer& getWriter() {
      return m_writer;
    }

    Reader& getReader() {
      return m_reader;
    }

    void sleepMs(int time) {
      this_thread::sleep_for(chrono::milliseconds(time));
    }
    void setCurrentTime(unsigned long time) {
      currentTime = time;
    }

    unsigned long getCurrentTime() {
      return currentTime;
    }

  protected:
    ConsoleWriter m_writer;
    IOReader m_reader;
    unsigned long currentTime = 0;
    
};

#endif
