#ifndef _INGREDIENTS_CONTROLLER_H
#define _INGREDIENTS_CONTROLLER_H

#include "controllers.h"

using HttpServer = SimpleWeb::Server<SimpleWeb::HTTP>;

using namespace NL::Template;
using namespace std;

namespace Ingredients {

  void show(WebProgram& wp, Template& t) {
    t.load("frontend/ingredients/show.html");
  }

  void index(WebProgram& wp, Template& t) {
    
    t.load("frontend/ingredients/index.html");

    int size = wp.ingredients.size();
    t.block("ingredients").repeat(size);

    for ( int i=0; i < size; i++ ) {

      Ingredient ingredient = wp.ingredients[i];
      t.block("ingredients")[i].set("name", ingredient.name);
      t.block("ingredients")[i].set("aliment_id", ingredient.aliment_id);
    }
  }
  
  void create(WebProgram& wp, Template& t) {
    t.load("frontend/ingredients/new.html");
  }

  void do_create(WebProgram& wp, PostRequest request) {

    string name = request.getMandatoryVal("name");
    string aliment_id = request.getVal("aliment_id");

    std::cout << "Do_create name: " << name << std::endl;
    std::cout << "Do_create aliment_id: " << aliment_id << std::endl;

    Ingredient ingredient(name, aliment_id);
    wp.ingredients.push_back(ingredient);
  }

}

#endif
