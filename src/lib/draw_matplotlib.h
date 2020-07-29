#ifndef _DRAW_MATPLOT_LIB_H
#define _DRAW_MATPLOT_LIB_H

#include "matplotlibcpp.h"

namespace plt = matplotlibcpp;

vector<double> toVect(double x) {
  vector<double> xs(1);
  xs[0] = x;
  return xs;
}

void beforeRenderScene() {
  plt::clf();
}

void renderScene(double xMin, double xMax, double yMin, double yMax, const char* title) {

  plt::xlim(xMin, xMax);
  plt::ylim(yMin, yMax);
  plt::zlim(0, 10);

  plt::title("Position du bras");
  plt::pause(0.01);
}

void drawPoint(double x, double y, const char* style="ro") {
  plt::plot(toVect(x),toVect(y),style);
}

void drawLines(vector<double> x, vector<double> y, const char* style="b-") {
  plt::plot(x,y,style);
}

#endif
