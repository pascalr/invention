#ifndef _MODEL_H
#define _MODEL_H

#include "SQLiteCpp/SQLiteCpp.h"
#include "position.h"
#include <vector>
#include <iostream>

#include "../lib/opencv.h"
#include "../lib/hr_code.h"

class WrongModelTypeException : public exception {};

template<class T>
class Table {
  public:

    virtual string getTableName() = 0;
    virtual T parseItem(SQLite::Statement& query) = 0;
    virtual string getValues(const T& item) = 0;
    virtual void bindBlob(SQLite::Statement& query, const T& item) {
    }

    bool exists = false;

    std::vector<T> items;
};

class Model {
  public:
    Model() {
      time_t time; localtime(&time);
      created_at = time;
      updated_at = time;
    }
    long id = -1;
    time_t created_at;
    time_t updated_at;
};

class DetectedHRCode : public Model {
  public:
    DetectedHRCode() {}
    DetectedHRCode(const HRCode& code, PolarCoord coord) : coord(coord), centerX(code.x), centerY(code.y), scale(code.scale), imgFilename(code.imgFilename) {
    }
    PolarCoord coord;
    double centerX;
    double centerY;
    double scale;
    string imgFilename;
    string jar_id;
    string weight;
    string content_name;
    string content_id;
};
//ostream &operator<<(std::ostream &os, const DetectedHRCode &c) {
//  return os << c.code << " at " << c.coord;
//}

class DetectedHRCodeTable : public Table<DetectedHRCode> {
  public:
    const char* TABLE_NAME = "detected_codes";
    string getTableName() { return TABLE_NAME; };
    
    string getValues(const DetectedHRCode& item) {
      stringstream ss; ss << item.coord(0) << ", " << item.coord(1) << ", " << item.coord(2) << ", ";
      ss << item.centerX << ", " << item.centerY << ", " << item.scale << ", '" << item.imgFilename << "', ";
      ss << item.created_at << ", " << item.updated_at << ", '" << item.jar_id << "', '" << item.weight << "', '";
      ss << item.content_name << "', '" << item.content_id << "'";
      return ss.str();
    }
    
    virtual void bindBlob(SQLite::Statement& query, const DetectedHRCode& item) {
      /*std::vector<uchar> buf;
      imencode(".jpeg", item.img, buf);
      char* blob = reinterpret_cast<char*>(buf.data());
      query.bind(1, blob, buf.size());
      */
    }

    /*
    0|id|integer|1||1
    1|x|float|0|NULL|0
    2|y|float|0|NULL|0
    3|t|float|0|NULL|0
    4|centerX|float|0|NULL|0
    5|centerY|float|0|NULL|0
    6|scale|float|0|NULL|0
    7|img|varchar|0||0
    8|created_at|datetime|0||0
    9|updated_at|datetime|0||0
    10|jar_id|varchar|0||0
    11|weight|varchar|0||0
    12|content_name|varchar|0||0
    13|content_id|varchar|0||0
    */

    // Maybe one day generate this all automatically from ruby?
    
    DetectedHRCode parseItem(SQLite::Statement& query) {
      DetectedHRCode code;
      code.id = query.getColumn(0);
      code.coord << query.getColumn(1), query.getColumn(2), query.getColumn(3);
      code.centerX = query.getColumn(4);
      code.centerY = query.getColumn(5);
      code.scale = query.getColumn(6);
      code.imgFilename = (const char*)query.getColumn(7);
      code.created_at = query.getColumn(8);
      code.updated_at = query.getColumn(9);
      code.jar_id = (const char*)query.getColumn(10);
      code.weight = (const char*)query.getColumn(11);
      code.content_name = (const char*)query.getColumn(12);
      code.content_id = (const char*)query.getColumn(13);
      return code;
    }

};

class JarFormat : public Model {
  public:
    double empty_weight;
    double height;
    double width;
    string name;
};

class Jar : public Model {
  public:
    PolarCoord position;
};

class JarTable : public Table<Jar> {
  public:
    const char* TABLE_NAME = "jars";
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
    const char* TABLE_NAME = "shelves";
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
};

#endif
