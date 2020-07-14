#include <iostream>
#include <cstring>
#include <math.h>
#include <string>

#include "utils/io_common.h"
#include "utils/utils.h"
#include "lib/matplotlibcpp.h"
#include "lib/lib.h"
#include "core/axis.h"
#include "core/console_writer.h"
#include "core/io_program.h"
#include "core/input_parser.h"
#include "config/setup.h"
      
using namespace std;
namespace plt = matplotlibcpp;

vector<double> toVect(double x) {
  vector<double> xs(1);
  xs[0] = x;
  return xs;
}

void drawToolPosition(double x, double z) {
  plt::plot(toVect(x),toVect(z),"ro");
}

void drawArmCenterAxis(Program& p) {
  vector<double> x(2);
  vector<double> z(2);

  x[0] = p.axisX.getPosition();
  x[1] = p.baseAxisX.getPosition();
  z[0] = p.axisZ.getPosition();
  z[1] = 0.0;
  plt::plot(x,z,"b-");
}

// TODO: Do my own thing I think, because yeah not sure what lib to use anyways...
// I will try to use Eigen333
class Point {
  public:
    Point(double valX, double valY) : x(valX), y(valY) {}
    Point operator+(Point& v) {
      return Point(x + v.x, y+v.y);
    }

    double x;
    double y;
};

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
  plt::plot(x,z,"k-");
}

void draw(Program& p) {
  
  plt::clf();
  
  drawPossibleXPosition(p.axisX.getMaxPosition());
  drawArmCenterAxis(p);
  drawToolPosition(p.axisX.getPosition(),p.axisZ.getPosition());

  plt::xlim(-OFFSET_X, ARMOIRE_WIDTH-OFFSET_X);
  plt::ylim(-OFFSET_Z, ARMOIRE_DEPTH-OFFSET_Z);

  plt::title("Position du bras");
  plt::pause(0.01);
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
