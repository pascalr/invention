#ifndef _WEB_PROGRAM_H
#define _WEB_PROGRAM_H

#include "Recette.h"
#include <vector>
#include "Database.h"

using namespace std;

class WebProgram {
  public:
    WebProgram() : db("data/test.db") {
    }

    //vector<Aliment> aliments;
    //vector<Ingredient> ingredients;
    //vector<Recette> recettes;
    Database db;
};

#endif
