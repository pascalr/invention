#ifndef _MODEL_H
#define _MODEL_H

#include "SQLiteCpp/SQLiteCpp.h"
#include "position.h"
#include <vector>

class WrongModelTypeException : public exception {};

template<class T>
class Table {
  public:

    virtual string getTableName() = 0;
    virtual T parseItem(SQLite::Statement& query) = 0;
    virtual string getValues(const T& item) = 0;

    bool exists = false;

    std::vector<T> items;
};

class Model {
  public:
    long rowid = -1;
};

class JarFormat : public Model {
  public:
    double empty_weight;
    double height;
    double width;
};

class Jar : public Model {
  public:
    PolarCoord position;
};

class JarTable : public Table<Jar> {
  public:
    const char* TABLE_NAME = "jar";
    string getTableName() { return TABLE_NAME; };
    
    string getValues(const Jar& item) {
      stringstream ss; ss << item.position(0) << ", " << item.position(1) << ", " << item.position(2);
      return ss.str();
    }
    
    Jar parseItem(SQLite::Statement& query) {
      Jar jar;
      double x = query.getColumn(1);
      double y = query.getColumn(2);
      double t = query.getColumn(3);
      jar.position << x, y, t;
      return jar;
    }

};


class Shelf : public Model {
  public:
    double height;
    double width;
    double depth;
    int is_working_shelf;
};

class ShelfTable : public Table<Shelf> {
  public:
    const char* TABLE_NAME = "shelf";
    string getTableName() { return TABLE_NAME; };
    
    string getValues(const Shelf& item) {
      stringstream ss; ss << item.height << ", " << item.width << ", " << item.depth << ", " << item.is_working_shelf;
      return ss.str();
    }
    
    Shelf parseItem(SQLite::Statement& query) {
      Shelf i;
      i.height = query.getColumn(1);
      i.width = query.getColumn(2);
      i.depth = query.getColumn(3);
      i.is_working_shelf = query.getColumn(4);
      return i;
    }

};

/*class Machine {
  vector<Shelf> shelves;
};*/

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
