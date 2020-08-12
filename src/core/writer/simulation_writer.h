#ifndef SIMULATION_WRITER 
#define SIMLUATION_WRITER

#include <iostream>
#include "std_writer.h"
#include <string>

class SimulationWriter : public StdWriter {
  public:
    void doPrint(std::string val) {
      std::cout << val;
    }
};

#endif
