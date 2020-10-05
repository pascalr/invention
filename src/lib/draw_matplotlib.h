#ifndef _DRAW_MATPLOT_LIB_H
#define _DRAW_MATPLOT_LIB_H

#include "matplotlibcpp.h"

namespace plt = matplotlibcpp;

std::vector<double> toVect(double x) {
  std::vector<double> xs(1);
  xs[0] = x;
  return xs;
}

void beforeRenderScene() {
  plt::clf();
  plt::figure_size(1200, 780);
}

void renderScene(double xMin, double xMax, double yMin, double yMax, const char* title) {

  plt::axis("equal");
  plt::xlim(xMin, xMax);
  plt::ylim(yMin, yMax);

  plt::title("Position du bras");
  plt::pause(0.01);
}

void show() {
  plt::show();
}

void drawPoint(double x, double y, const char* style="ro") {
  plt::plot(toVect(x),toVect(y),style);
}

void drawLines(std::vector<double> x, std::vector<double> y, const char* style="b-") {
  plt::plot(x,y,style);
}

#endif
