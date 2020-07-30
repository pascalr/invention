#ifndef _3D_CONTROLLER_H
#define _3D_CONTROLLER_H

#include "controllers.h"

using HttpServer = SimpleWeb::Server<SimpleWeb::HTTP>;
using namespace NL::Template;
using namespace std;

namespace ThreeD {

  void index(WebProgram& wp, Template& t) {
    
    t.load("frontend/3d/index.html");
  }
  
}

#endif
