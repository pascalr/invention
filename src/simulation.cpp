#include <iostream>
#include <cstring>
#include <math.h>
#include <string>

#include "utils/io_common.h"
#include "utils/utils.h"
#include "lib/lib.h"
#include "core/two_way_stream.h"
#include "core/reader/io_reader.h"
#include "core/input_parser.h"
#include "core/fake_program.h"
#include "lib/draw_matplotlib.h"
#include "core/heda.h"
#include "core/writer/log_writer.h"
#include "core/writer/console_writer.h"
#include "core/heda_controller.h"
      
using namespace std;


void drawToolPosition(double x, double z) {
  drawPoint(x, z);
}

/*void drawArmCenterAxis(Program& p) {
  vector<double> x(2);
  vector<double> z(2);

  x[0] = p.baseAxisX.getPosition();
  x[1] = p.baseAxisX.getPosition();
  z[0] = p.axisZ.getPosition();
  z[1] = 0.0;
  drawLines(x,z);
}*/

void drawPossibleXPosition(double maxX) {
  vector<double> x(2);
  vector<double> z(2);
  x[0] = 0.0;
  x[1] = maxX;
  z[0] = 0.0;
  z[1] = 0.0;
  drawLines(x,z,"k-");
}

void draw(Program& p) {
 
  beforeRenderScene(); 
  
  drawPossibleXPosition(p.baseAxisX.getMaxPosition());
  //drawArmCenterAxis(p);
  //drawToolPosition(p.baseAxisX.getPosition(),p.axisZ.getPosition());

  renderScene(-OFFSET_X, ARMOIRE_WIDTH-OFFSET_X, -OFFSET_Z, ARMOIRE_DEPTH-OFFSET_Z, "Position du bras");
}

void mySignalHandler( int signum ) {
  std::cout << "Interrupt signal (" << signum << ") received.\n";
  this_thread::sleep_for(chrono::milliseconds(100));
  std::terminate();
  exit(signum);
}

int main (int argc, char *argv[]) {

  signal(SIGINT, mySignalHandler);

  Database db(DB_TEST);

  TwoWayStream hedaInput;
  TwoWayStream hedaOutput;

  LogWriter simulationOut = LogWriter("Simulation output", hedaInput);
  LogWriter hedaOut = LogWriter("Heda output", hedaOutput);

  ConsoleWriter hedaOutputWriter;

  FakeProgram simulation(simulationOut, hedaOutput); // writer, reader

  TwoWayStream hedaToServerStream;
  LogWriter hedaToServerLogStream("\033[37mTo server\033[0m", hedaToServerStream);

  Heda heda(hedaOut, hedaInput, db, hedaOutputWriter); 

  thread simulation_thread([&]() {
    while (true) {
      draw(simulation);
      this_thread::sleep_for(chrono::milliseconds(20));
    }
  });

  HedaController c(heda);
  IOReader reader;
  c.loopCommandStack(reader);

  /*
  IOReader userInput;
  cerr << ">> ";
  draw(simulation);
  while (true) {
    bool wasWorking = simulation.isWorking;

    if (userInput.inputAvailable()) {
      string cmd = getInputLine(userInput);
      heda.execute(cmd);
    }

    if (simulation.isWorking && simulation.getCurrentTime() % 200000 == 0) {
      draw(simulation);
    }
    if (wasWorking && !simulation.isWorking) {
      cerr << ">> ";
      draw(simulation);
    }
    
    this_thread::sleep_for(chrono::milliseconds(10));
  }*/

  return 0;
}
