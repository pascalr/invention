#ifndef _DATABASE_H
#define _DATABASE_H

#include "SQLiteCpp/SQLiteCpp.h"
#include "Recette.h"
#include <vector>

using namespace std;

class Database {
  public:

    Database(const char* dbName) : db(dbName, SQLite::OPEN_READWRITE|SQLite::OPEN_CREATE) {
    }

    void load(vector<Ingredient>& ingredients) {

      SQLite::Statement query(db, "SELECT * FROM ingredient");
      
      while (query.executeStep())
      {
        const char* name            = query.getColumn(0);
        int         aliment_id      = query.getColumn(1);
        Ingredient ingredient(name, aliment_id);
        ingredients.push_back(ingredient);
      }
    }

    void save(Ingredient ingredient) {
      db.exec("INSERT INTO ingredient VALUES (\""+ingredient.name+"\", "+to_string(ingredient.aliment_id)+")");
    }

    SQLite::Database db;
};

#endif
