#ifndef FAKE_PROGRAM_H
#define FAKE_PROGRAM_H

#include "program.h"
#include "reader/fake_reader.h"
#include "writer/console_writer.h"

class FakeProgram : public Program {
  public:
    FakeProgram(Writer& writer0, Reader& reader0) : Program(writer0, reader0), writer(writer0), reader(reader0) {}

    Writer& getWriter() {
      return writer;
    }

    Reader& getReader() {
      return reader;
    } 

    void sleepMs(int time);

    void setCurrentTime(unsigned long time) {
      currentTime = time;
    }

    void move(char axis, double destination);

    unsigned long currentTime = 0;

    unsigned long getCurrentTime() {
      return currentTime;
    }

    Writer& writer;
    Reader& reader;
};


#endif
