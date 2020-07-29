#ifndef _RECETTE_H
#define _RECETTE_H

using namespace std;

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
