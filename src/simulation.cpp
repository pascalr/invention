#include <iostream>
#include <cstring>
#include <math.h>
#include <string>

#include "utils/io_common.h"
#include "utils/utils.h"
#include "lib/lib.h"
#include "core/axis.h"
#include "core/console_writer.h"
#include "core/io_program.h"
#include "core/input_parser.h"
#include "config/setup.h"
#include "lib/draw_matplotlib.h"
      
using namespace std;


void drawToolPosition(double x, double z) {
  drawPoint(x, z);
}

void drawArmCenterAxis(Program& p) {
  vector<double> x(2);
  vector<double> z(2);

  x[0] = p.axisX.getPosition();
  x[1] = p.baseAxisX.getPosition();
  z[0] = p.axisZ.getPosition();
  z[1] = 0.0;
  drawLines(x,z);
}

/*void drawArmBoundingBox(HorizontalAxis* axisX, ZAxis* axisZ) {
  double angle = axisZ->getPositionAngle();

  double a[4][2] = {{0,0},
                   {ARM_WIDTH, 0},
                   {ARM_WIDTH, ARM_LENGTH},
                   {0, ARM_LENGTH}};

} */

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
  
  drawPossibleXPosition(p.axisX.getMaxPosition());
  drawArmCenterAxis(p);
  drawToolPosition(p.axisX.getPosition(),p.axisZ.getPosition());

  renderScene(-OFFSET_X, ARMOIRE_WIDTH-OFFSET_X, -OFFSET_Z, ARMOIRE_DEPTH-OFFSET_Z, "Position du bras");
}

int main (int argc, char *argv[]) {

  signal(SIGINT, signalHandler);

  IOProgram p;
  setupAxes(p);

  cerr << ">> ";
  draw(p);

  while (true) {
    bool wasWorking = p.isWorking;

    p.setCurrentTime((p.isWorking) ? p.getCurrentTime() + 5 : 0);

    myLoop(p);
    
    if (p.isWorking && p.getCurrentTime() % 200000 == 0) {
      draw(p);
    }
    if (wasWorking && !p.isWorking) {
      cerr << ">> ";
      draw(p);
    }
  }

  return 0;
}
