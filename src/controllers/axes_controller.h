#ifndef _AXES_CONTROLLER_H
#define _AXES_CONTROLLER_H

#include "../lib/NLTemplate.h"
#include "../core/program.h"

using namespace NL::Template;

namespace Axes {

  void index(WebProgram& wp, Template& t, Program& p) {
    t.load("frontend/axes/index.html");
    std::cout << "There are " << NUMBER_OF_AXES << " axes to prints.\n";
    //t.block("items").repeat(3);     // We need to know in advance that the "items" block will repeat 3 times.
  }

}

#endif
