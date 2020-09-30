#ifndef _MODEL_H
#define _MODEL_H

#include "SQLiteCpp/SQLiteCpp.h"
#include "position.h"
#include <vector>
#include <iostream>

#include "../lib/opencv.h"
#include "../lib/hr_code.h"

class WrongModelTypeException : public exception {};

// TODO: Validate column names indexes

template <class T>
int byJarId(const T& t) {
  return t.jar_id;
}
template <class T>
string byName(const T& t) {
  return t.name;
}
template <class T>
double byHeight(const T& t) {
  return t.height;
}
template <class T>
double byLidY(const T& t) {
  return t.lid_coord.y;
}
template <class T>
void order(std::vector<T>& items, double (*func)(const T&), bool ascending=true) {
  if (ascending) {
    std::sort(items.begin(), items.end(), [func](const T& arg0, const T& arg1){return func(arg0) < func(arg1);});
  } else {
    std::sort(items.rbegin(), items.rend(), [func](const T& arg0, const T& arg1){return func(arg0) < func(arg1);});
  }
}

//double byLidZ(const DetectedHRCode& t) {
//  return t.lid_coord.z;
//}

template<class T>
class Table {
  public:

    virtual string getTableName() = 0;
    virtual T parseItem(SQLite::Statement& query) = 0;
    //virtual string getValues(const T& item) = 0;
    virtual void bindQuery(SQLite::Statement& query, const T& item) = 0;
    //virtual void bindQuery(SQLite::Statement& query, const T& item, int i) = 0;
    bool get(T& item, int id) {
      for (auto it = items.begin(); it != items.end(); ++it) {
        if (it->id == id) {
          item = *it;
          return true;
        }
      }
      return false;
    }

    bool ifind(T& t, string (*func)(const T&), string cmp) {
      for (const T& i : items) {
        if (iequals(func(i), cmp)) {
          t = i;
          return true;
        }
      }
      return false;
    }

    template <class P>
    bool find(T& t, P (*func)(const T&), P cmp) {
      for (const T& i : items) {
        if (func(i) == cmp) {
          t = i;
          return true;
        }
      }
      return false;
    }
//    bool find(T& t, std::function<bool(const T&)> func) {
//      for (const T& i : items) {
//        if (func(i)) {
//          t = i;
//          return true;
//        }
//      }
//      return false;
//    }

    typename std::vector<T>::iterator begin() {
      return items.begin();
    }
    typename std::vector<T>::iterator end() {
      return items.end();
    }
    int size() {
      return items.size();
    }
    bool empty() {
      return items.empty();
    }
    T& back() {
      return items.back();
    }
    void order(double (*func)(const T&), bool ascending=true) {
      if (ascending) {
        std::sort(items.begin(), items.end(), [func](const T& arg0, const T& arg1){return func(arg0) < func(arg1);});
      } else {
        std::sort(items.rbegin(), items.rend(), [func](const T& arg0, const T& arg1){return func(arg0) < func(arg1);});
      }
    }
    /*void erase(std::vector<T>::iterator it) {
      items.erase(it);
    }*/
    // TODO: operator[]

    //bool exists = false;

    int column_count;
    std::string update_query;
    std::string insert_query;
    std::vector<T> items; // TODO: Make items protected

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

    bool exists() {
      return id != -1;
    }
    
    // This could be bad because of implicit conversion to other types from what I've read.
    //operator bool() const {
    //  return id == -1;
    //}
};

class HedaConfig : public Model {
  public:
    HedaConfig() {}

    long working_shelf_id;
    double user_coord_offset_x;
    double user_coord_offset_y;
    double user_coord_offset_z;
    double camera_radius;
    double gripper_radius;
    double camera_focal_point;
    double detect_height;
    double home_position_x;
    double home_position_y;
    double home_position_t;
    double grip_offset;
    double max_h;
    double max_v;
    double max_t;
    double closeup_distance;
    double max_x;
    double max_y;
    double max_z;

    double minX() {
      return 0.0;
    }
    double minY() {
      return 0.0;
    }
    double minZ() {
      return 0.0;
    }
    double minH() {
      return 0.0;
    }
    double minV() {
      return 0.0;
    }
    double minT() {
      return 0.0;
    }
    double middleX() {
      return max_x / 2.0;
    }
    double middleH() {
      return max_h / 2.0;
    }
};


void parseItem(SQLite::Statement& query, HedaConfig& c);
void bindQuery(SQLite::Statement& query, const HedaConfig& item);

class Location : public Model {
  public:
    Location() {}
    Location(double x, double z, string moveCommand, int shelfId, double diameter) : x(x), z(z), move_command(moveCommand), shelf_id(shelfId), diameter(diameter) {}

    double x;
    double z;
    string move_command;
    int shelf_id;

    double diameter;
    bool is_storage;
    string name;
    int jar_format_id;
    int jar_id;
    bool occupied;
};

void parseItem(SQLite::Statement& query, Location& item);
void bindQuery(SQLite::Statement& query, const Location& item);

class DetectedHRCode : public Model {
  public:
    DetectedHRCode() {}
    DetectedHRCode(const HRCode& code, PolarCoord coord) : coord(coord), centerX(code.x), centerY(code.y), scale(code.scale), imgFilename(code.imgFilename) {
    }
    /*DetectedHRCode(const DetectedHRCode& c) : coord(c.coord), centerX(c.centerX), centerY(c.centerY), scale(c.scale), imgFilename(c.imgFilename), jar_id(c.jar_id), weight(c.weight),
                                              content_name(c.content_name), content_id(c.content_id), lid_coord(c.lid_coord) {
      id = c.id;
    }*/

    PolarCoord coord;
    double centerX;
    double centerY;
    double scale;
    string imgFilename;
    string jar_id;
    string weight;
    string content_name;
    string content_id;
    UserCoord lid_coord;
};

void bindQuery(SQLite::Statement& query, const DetectedHRCode& item);
void parseItem(SQLite::Statement& query, DetectedHRCode& code);
//string getTableName(DetectedHRCode* code = NULL);
template<typename T>
string getTableName();

class JarFormat : public Model {
  public:
    double empty_weight;
    double height;
    double diameter;
    string name;
    double lid_diameter;
    double lid_weight;
    double grip_force;
};

void bindQuery(SQLite::Statement& query, const JarFormat& item);
void parseItem(SQLite::Statement& query, JarFormat& item);

class Jar : public Model {
  public:
    int jar_format_id = -1;
    int ingredient_id = -1;
    int jar_id = -1;
};

//string getTableName(Jar* code = NULL);
void bindQuery(SQLite::Statement& query, const Jar& item);
void parseItem(SQLite::Statement& query, Jar& item);

class Ingredient : public Model {
  public:
    string name;
    int aliment_id;
    double cost;
    double quantity;
    string unit_name;
    double density;
};

void bindQuery(SQLite::Statement& query, const Ingredient& item);
void parseItem(SQLite::Statement& query, Ingredient& i);

class Unit : public Model {
  public:
    string name;
    double value;
    bool is_weight;
};

void bindQuery(SQLite::Statement& query, const Unit& item);
void parseItem(SQLite::Statement& query, Unit& i);

class Recipe : public Model {
  public:
    string name;
    string instructions;
    double rating;
};

void bindQuery(SQLite::Statement& query, const Recipe& item);
void parseItem(SQLite::Statement& query, Recipe& i);

class IngredientQuantity : public Model {
  public:
    int recipe_id;
    int ingredient_id;
    double value;
    int unit_id;
};

void bindQuery(SQLite::Statement& query, const IngredientQuantity& item);
void parseItem(SQLite::Statement& query, IngredientQuantity& i);

class Shelf : public Model {
  public:
    double height;
    double width;
    double depth;
    double offset_x; // deprecated
    double offset_z; // deprecated
    double moving_height;
};
    
void bindQuery(SQLite::Statement& query, const Shelf& item);
void parseItem(SQLite::Statement& query, Shelf& i);

#endif
