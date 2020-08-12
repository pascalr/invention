#ifndef FAKE_PROGRAM_H
#define FAKE_PROGRAM_H

#include "program.h"
#include "reader/reader.h"
#include "writer/writer.h"

class FakeProgram : public Program {
  public:
    FakeProgram(Reader& reader, Writer& writer) : Program(writer, reader), m_reader(reader), m_writer(writer) {
    }

    Writer& getWriter() {
      return m_writer;
    }

    Reader& getReader() {
      return m_reader;
    } 

    /*void setFakeInput(const char* str) {
      m_reader.setFakeInput(str);
    }
    
    void setFakeInput(std::string& str) {
      m_reader.setFakeInput(str);
    }*/

    void sleepMs(int time);

    void setCurrentTime(unsigned long time) {
      currentTime = time;
    }

    //void execute(const char* dest);

    //void move(char axis, double destination);

    unsigned long currentTime = 0;

    unsigned long getCurrentTime() {
      return currentTime;
    }

  protected:

    Reader& m_reader;
    Writer& m_writer;
};


#endif
