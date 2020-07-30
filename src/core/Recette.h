#ifndef _RECETTE_H
#define _RECETTE_H

#include "Model.h"

using namespace std;

// A unique aliment. I provide the list of aliment.
// They have all the information, nutrional value, etc...
class Aliment {
  // sugar
  // protein
  // fat
  // volum mass
  // ...
};

// Ingredients in recipees are referred to by name.
// You have your list of ingredients, you put anything in any language.
// You can search and map your ingredient to a unique id with a search tool.
class Ingredient : public Model {
  public:
    Ingredient(string name) : name(name) {}
    Ingredient(string name, int aliment_id) : name(name), aliment_id(aliment_id) {}

    string name;
    int aliment_id = 0;
    long rowid = 0;
};

/*
// Liquid is in ml, solid is in g
enum IngredientType { LIQUID, SOLID };

class Ingredient {
  public:
    int id;
    string name;
    IngredientType type;
    double volumic_mass;

    void saveAsHtml() {
    }

    void saveAsJson() {
    }
};
*/

class Recette {
  public:

    Recette() {
    }

    Recette(string name) : name(name) {
    }

    Recette(string name, string instructions) : name(name), instructions(instructions) {
    }

    string name;
    string instructions;
    long rowid = 0;
};


#endif
