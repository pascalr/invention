#ifndef _DATABASE_H
#define _DATABASE_H

#include "SQLiteCpp/SQLiteCpp.h"
#include "Recette.h"
#include <vector>

using namespace std;

// HUUUUUUUUGEEEE FIXME: I do not validate the user input. It could insert sql...
// I should probably use ruby on rails which does all this already...

#include <mutex>

std::mutex dbMutex;

class Database {
  public:

    Database(const char* dbName) : db(dbName, SQLite::OPEN_READWRITE|SQLite::OPEN_CREATE) {
    }

    void load(vector<Ingredient>& ingredients) {
      std::lock_guard<std::mutex> guard(dbMutex);

      SQLite::Statement query(db, "SELECT rowid,* FROM ingredient");
      
      while (query.executeStep())
      {
        const char* name            = query.getColumn(1);
        int         aliment_id      = query.getColumn(2);
        Ingredient ingredient(name, aliment_id);
        ingredient.rowid = query.getColumn(0);
        ingredients.push_back(ingredient);
      }
    }

    void save(Ingredient ingredient) {
      std::lock_guard<std::mutex> guard(dbMutex);
      db.exec("INSERT INTO ingredient VALUES (\""+ingredient.name+"\", "+to_string(ingredient.aliment_id)+")");
    }

    void loadRecette(SQLite::Statement& query, Recette& recette) {
      std::lock_guard<std::mutex> guard(dbMutex);

      recette.rowid = query.getColumn(0);
      recette.name = (const char*)query.getColumn(1);
      recette.instructions = (const char*)query.getColumn(2);
    }

    void load(vector<Recette>& recettes) {
      std::lock_guard<std::mutex> guard(dbMutex);

      SQLite::Statement query(db, "SELECT rowid,* FROM recette");
      
      while (query.executeStep())
      {
        Recette recette;
        loadRecette(query, recette);
        recettes.push_back(recette);
      }
    }

    void remove(Recette& recette, string name) {
      std::lock_guard<std::mutex> guard(dbMutex);
      db.exec("DELETE FROM recette WHERE name =  \""+name+"\"");
    }

    void get(Recette& recette, string name) {
      std::lock_guard<std::mutex> guard(dbMutex);

      SQLite::Statement query(db, "SELECT rowid,* FROM recette WHERE NAME = \"" + name + "\"");
      query.executeStep();
      loadRecette(query, recette);
    }

    void save(Recette recette) {
      std::lock_guard<std::mutex> guard(dbMutex);
      if (recette.rowid) {
        cout << "Updating recipee\n";
        db.exec("UPDATE recette SET name = \""+recette.name+"\", instructions = \""+recette.instructions+"\" WHERE rowid = " + to_string(recette.rowid));
      } else {
        cout << "Saving recipee\n";
        db.exec("INSERT INTO recette VALUES (\""+recette.name+"\", \""+recette.instructions+"\")");
      }
    }

    SQLite::Database db;
};

#endif
