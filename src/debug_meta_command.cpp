#include <iostream>

#include "core/heda.h"
      
using namespace std;

void drawArm(Heda& heda, Program& p) {
  vector<double> x(2);
  vector<double> z(2);

  UserCoord basePos = heda.toUserCoord(PolarCoord(p.baseAxisX.getPosition(),0,0), 0.0);
  UserCoord toolPosition = heda.toUserCoord(PolarCoord(p.baseAxisX.getPosition(),0,p.axisT.getPosition()), heda.config.gripper_radius);

  x[0] = basePos.x;
  x[1] = toolPosition.x;
  z[0] = basePos.z;
  z[1] = toolPosition.z;
  drawLines(x,z);
}

void drawAxisH(Heda& heda) {
  vector<double> x(2);
  vector<double> z(2);
  x[0] = heda.config.user_coord_offset_x;
  z[0] = heda.config.user_coord_offset_z;
  x[1] = heda.config.user_coord_offset_x + heda.config.max_h;
  z[1] = heda.config.user_coord_offset_z;
  drawLines(x,z,"k-");
}

void draw(Program& p, Heda& heda) {
 
  beforeRenderScene(); 
  
  drawAxisH(heda);
  
  drawArm(heda, p);
  
  UserCoord toolPosition = heda.toUserCoord(PolarCoord(p.baseAxisX.getPosition(),0,p.axisT.getPosition()), heda.config.gripper_radius);
  drawPoint(toolPosition.x, toolPosition.z);

  //drawToolPosition(p.baseAxisX.getPosition(),p.axisZ.getPosition());

  double deltaZ = heda.config.user_coord_offset_z - heda.working_shelf.depth;
  double z = heda.config.user_coord_offset_z + deltaZ;
  renderScene(0, heda.working_shelf.width, 0, z, "Position du bras");
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
  setupAxes(simulation);

  TwoWayStream hedaToServerStream;
  LogWriter hedaToServerLogStream("\033[37mTo server\033[0m", hedaToServerStream);

  Heda heda(hedaOut, hedaInput, db, hedaOutputWriter); 

  thread simulation_thread([&]() {
    while (true) {
      draw(simulation, heda);
      this_thread::sleep_for(chrono::milliseconds(50));
    }
  });

  HedaController c(heda);

  simulation.setCurrentTime(1000);

  IOReader reader;
  while (true) {
    if(reader.inputAvailable()) {
      string cmd = getInputLine(reader);
      c.execute(cmd);
    }
    //for (int i = 0; i < 1000; i++) {
    //}
    simulation.setCurrentTime(simulation.getCurrentTime() + 20000);
    myLoop(simulation);
    heda.handleCommandStack();
    this_thread::sleep_for(chrono::milliseconds(1));
  }
  

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
