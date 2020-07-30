#ifndef _WEB_PROGRAM_H
#define _WEB_PROGRAM_H

#include "Recette.h"
#include <vector>

using namespace std;

class WebProgram {
  public:
    vector<Aliment> aliments;
    vector<Ingredient> ingredients;
    vector<Recette> recettes;
};

#endif
