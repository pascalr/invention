#ifndef _AXES_CONTROLLER_H
#define _AXES_CONTROLLER_H

#include "../lib/NLTemplate.h"

using namespace NL::Template;

namespace Axes {

  void index(Template& t) {
    t.load("frontend/axes/index.html");
  }

}

#endif
