#ifndef _RECETTE_H
#define _RECETTE_H

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
class Ingredient {
  public:
    Ingredient(string name) : name(name) {}
    Ingredient(string name, string aliment_id) : name(name), aliment_id(aliment_id) {}

    string name;
    int aliment_id;
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

class Instruction {
};

class Recette {
  public:

    void saveAsHtml() {
    }

    void saveAsJson() {
    }

    //friend ostream &operator<<(std::ostream &os, const Recette &r);

  protected:

    int m_version = 0;
    vector<Ingredient> m_ingredients; // Only the id is saved and loaded.
    vector<double> m_quantities;
    vector<Instruction> m_instructions;

};


#endif
