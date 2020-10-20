#include "core/model.h"
#include "core/database.h"
#include "core/recipe_parser.h"
#include <iostream>

int main(int argc, char** argv) {

  if (argc < 2) {
    std::cerr << "Not enough arguments given.\n";
    std::cerr << "Syntax: recipe_parser path/to_data/base.sqlite3 (recipe id)\n";
    return -1;
  }
  Database db(argv[1]);
 
  try {
    if (argc < 3) {

      std::cout << "No recipe id given. Parse all the recipes.\n";
      std::vector<Recipe> recipes = db.all<Recipe>();
      for (Recipe& recipe : recipes) {
        parseRecipe(db, recipe);
      }

    } else if (argc == 3) {
      std::cout << "Parsing recipe with id = " << argv[2] << "\n";
      int id = atof(argv[2]);
      Recipe recipe = db.find<Recipe>(id);
      parseRecipe(db, recipe);

    } else {
      std::cerr << "Too many arguments given.\n";
      std::cerr << "Syntax: recipe_parser path/to_data/base.sqlite3 (recipe id)\n";
      return -1;
    }
  } catch (const EnsureException& e) {
    return -1;
  }

  return 0;
}
