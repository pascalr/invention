#ifndef _INGREDIENTS_CONTROLLER_H
#define _INGREDIENTS_CONTROLLER_H

#include "controllers.h"

using HttpServer = SimpleWeb::Server<SimpleWeb::HTTP>;

using namespace NL::Template;
using namespace std;

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

  void do_create(WebProgram& wp, PostRequest request) {

    string name = request.getMandatoryVal("name");
    string fdc_id = request.getVal("fdc_id");

    std::cout << "Do_create name: " << name << std::endl;
    std::cout << "Do_create id: " << fdc_id << std::endl;
    
    //wp.ingredients.push_back
  }

}

#endif
