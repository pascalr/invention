#ifndef _INGREDIENTS_CONTROLLER_H
#define _INGREDIENTS_CONTROLLER_H

#include "../lib/NLTemplate.h"

using namespace NL::Template;

namespace Ingredients {

  void show(Template& t) {
    t.load("frontend/ingredients/show.html");
  }

  void index(Template& t) {
    t.load("frontend/ingredients/index.html");
  }
  
  void create(Template& t) {
    t.load("frontend/ingredients/new.html");
  }

}

#endif
