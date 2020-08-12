#ifndef SIMULATION_PROGRAM_H
#define SIMULATION_PROGRAM_H

#include "program.h"
#include "reader/reader.h"
#include "writer/writer.h"
#include "input_parser.h"
#include <thread>
#include <chrono>

class Simulation : public Program {
  public:
    Simulation(Reader& reader, Writer& writer) : Program(writer, reader), m_reader(reader), m_writer(writer) {
      cout << "Starting simulation...\n";
      m_simulation_thread_run = true;
      m_simulation_thread = std::thread(&Simulation::runSimulation, this);
    }

    ~Simulation() {
      cout << "Ending simulation...\n";
      m_simulation_thread_run = false;
      m_simulation_thread.join();
    }

    void runSimulation() {
      setCurrentTime(0);
      myLoop(*this);
      while (isWorking) {
        setCurrentTime(currentTime + 5);
        myLoop(*this);
      }
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

    unsigned long currentTime = 0;

    unsigned long getCurrentTime() {
      return currentTime;
    }

  protected:

    Reader& m_reader;
    Writer& m_writer;
    std::thread m_simulation_thread;
    bool m_simulation_thread_run;
};

#endif
