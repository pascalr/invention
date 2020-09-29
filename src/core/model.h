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

class HedaConfigTable : public Table<HedaConfig> {
  public:
    const char* TABLE_NAME = "hedas";
    string getTableName() { return TABLE_NAME; };

    HedaConfig parseItem(SQLite::Statement& query) {
      HedaConfig c;
      c.working_shelf_id = query.getColumn(1);
      c.created_at = query.getColumn(2);
      c.updated_at = query.getColumn(3);
      c.user_coord_offset_x = query.getColumn(4);
      c.user_coord_offset_y = query.getColumn(5);
      c.user_coord_offset_z = query.getColumn(6);
      c.camera_radius = query.getColumn(7);
      c.gripper_radius = query.getColumn(8);
      c.camera_focal_point = query.getColumn(9);
      c.detect_height = query.getColumn(10);
      c.home_position_x = query.getColumn(11);
      c.home_position_y = query.getColumn(12);
      c.home_position_t = query.getColumn(13);
      c.grip_offset = query.getColumn(14);
      c.max_h = query.getColumn(15);
      c.max_v = query.getColumn(16);
      c.max_t = query.getColumn(17);
      c.closeup_distance = query.getColumn(18);
      c.max_x = query.getColumn(19);
      c.max_y = query.getColumn(20);
      c.max_z = query.getColumn(21);
      return c;
    }

    void bindQuery(SQLite::Statement& query, const HedaConfig& item) {
      query.bind(1, item.working_shelf_id);
      query.bind(2, item.created_at);
      query.bind(3, item.updated_at);
      query.bind(4, item.user_coord_offset_x);
      query.bind(5, item.user_coord_offset_y);
      query.bind(6, item.user_coord_offset_z);
      query.bind(7, item.camera_radius);
      query.bind(8, item.gripper_radius);
      query.bind(9, item.camera_focal_point);
      query.bind(10, item.detect_height);
      query.bind(11, item.home_position_x);
      query.bind(12, item.home_position_y);
      query.bind(13, item.home_position_t);
      query.bind(14, item.grip_offset);
      query.bind(15, item.max_h);
      query.bind(16, item.max_v);
      query.bind(17, item.max_t);
      query.bind(18, item.closeup_distance);
      query.bind(19, item.max_x);
      query.bind(20, item.max_y);
      query.bind(21, item.max_z);
    }
};

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

class LocationTable : public Table<Location> {
  public:
    const char* TABLE_NAME = "locations";
    string getTableName() { return TABLE_NAME; };

    Location parseItem(SQLite::Statement& query) {
      Location item;
      item.x = query.getColumn(1);
      item.z = query.getColumn(2);
      item.move_command = (const char*)query.getColumn(3);
      item.shelf_id = query.getColumn(4);
      item.created_at = query.getColumn(5);
      item.updated_at = query.getColumn(6);
      item.diameter = query.getColumn(7);
      item.is_storage = (int)query.getColumn(8);
      item.name = (const char*)query.getColumn(9);
      item.jar_format_id = query.getColumn(11);
      item.jar_id = query.getColumn(12);
      item.occupied = (int)query.getColumn(13);
      return item;
    }

    void bindQuery(SQLite::Statement& query, const Location& item) {
      query.bind(1, item.x);
      query.bind(2, item.z);
      query.bind(3, item.move_command);
      query.bind(4, item.shelf_id);
      query.bind(5, item.created_at);
      query.bind(6, item.updated_at);
      query.bind(7, item.diameter);
      query.bind(8, item.is_storage);
      query.bind(9, item.name);
      query.bind(11, item.jar_format_id);
      query.bind(12, item.jar_id);
      query.bind(13, item.occupied);
    }

    int byName(Location& loc, const string& name) {
      for (const Location& l : items) {
        if (iequals(l.name, name)) {
          loc = l;
          return 1;
        }
      }
      return -1;
    }
};


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

class DetectedHRCodeTable : public Table<DetectedHRCode> {
  public:
    const char* TABLE_NAME = "detected_codes";
    string getTableName() { return TABLE_NAME; };
    
    void bindQuery(SQLite::Statement& query, const DetectedHRCode& item) {
      query.bind(1, item.coord.h);
      query.bind(2, item.coord.v);
      query.bind(3, item.coord.t);
      query.bind(4, item.centerX);
      query.bind(5, item.centerY);
      query.bind(6, item.scale);
      query.bind(7, item.imgFilename);
      query.bind(8, item.created_at);
      query.bind(9, item.updated_at);
      query.bind(10, item.jar_id);
      query.bind(11, item.weight);
      query.bind(12, item.content_name);
      query.bind(13, item.content_id);
      query.bind(14, item.lid_coord.x);
      query.bind(15, item.lid_coord.y);
      query.bind(16, item.lid_coord.z);
    }

    // Maybe one day generate this all automatically from ruby?
    
    DetectedHRCode parseItem(SQLite::Statement& query) {
      DetectedHRCode code;
      code.coord = PolarCoord(query.getColumn(1), query.getColumn(2), query.getColumn(3));
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
      code.lid_coord = UserCoord(query.getColumn(14), query.getColumn(15), query.getColumn(16));
      return code;
    }

};

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

class JarFormatTable : public Table<JarFormat> {
  public:
    const char* TABLE_NAME = "jar_formats";
    string getTableName() { return TABLE_NAME; };
    
    void bindQuery(SQLite::Statement& query, const JarFormat& item) {
      query.bind(1, item.empty_weight);
      query.bind(2, item.height);
      query.bind(3, item.diameter);
      query.bind(4, item.created_at);
      query.bind(5, item.updated_at);
      query.bind(6, item.name);
      query.bind(7, item.lid_diameter);
      query.bind(8, item.lid_weight);
      query.bind(9, item.grip_force);
    }

    JarFormat parseItem(SQLite::Statement& query) {
      JarFormat item;
      item.empty_weight = query.getColumn(1);
      item.height = query.getColumn(2);
      item.diameter = query.getColumn(3);
      item.created_at = query.getColumn(4);
      item.updated_at = query.getColumn(5);
      item.name = (const char*)query.getColumn(6);
      item.lid_diameter = query.getColumn(7);
      item.lid_weight = query.getColumn(8);
      item.grip_force = query.getColumn(9);
      return item;
    }

};

class Jar : public Model {
  public:
    int jar_format_id = -1;
    int ingredient_id = -1;
    int jar_id = -1;
};

//string getTableName(Jar* code = NULL);
void bindQuery(SQLite::Statement& query, const Jar& item);
void parseItem(SQLite::Statement& query, Jar& item);

class JarTable : public Table<Jar> {
  public:
    const char* TABLE_NAME = "jars";
    string getTableName() { return TABLE_NAME; };
    
    void bindQuery(SQLite::Statement& query, const Jar& item) {
      query.bind(1, item.jar_format_id);
      query.bind(2, item.ingredient_id);
      query.bind(3, item.created_at);
      query.bind(4, item.updated_at);
      query.bind(6, item.jar_id);
    }

    Jar parseItem(SQLite::Statement& query) {
      Jar jar;
      jar.jar_format_id = query.getColumn(1);
      jar.ingredient_id = query.getColumn(2);
      jar.created_at = query.getColumn(3);
      jar.updated_at = query.getColumn(4);
      jar.jar_id = query.getColumn(6);
      return jar;
    }

};

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

class IngredientTable : public Table<Ingredient> {
  public:
    const char* TABLE_NAME = "ingredients";
    string getTableName() { return TABLE_NAME; };
    
    void bindQuery(SQLite::Statement& query, const Ingredient& item) {
      query.bind(1, item.name);
      query.bind(2, item.aliment_id);
      query.bind(3, item.created_at);
      query.bind(4, item.updated_at);
      query.bind(5, item.cost);
      query.bind(6, item.quantity);
      query.bind(7, item.unit_name);
      query.bind(8, item.density);
    }
    
    Ingredient parseItem(SQLite::Statement& query) {
      Ingredient i;
      i.name = (const char*)query.getColumn(1);
      i.aliment_id = query.getColumn(2);
      i.created_at = query.getColumn(3);
      i.updated_at = query.getColumn(4);
      i.cost = query.getColumn(5);
      i.quantity = query.getColumn(6);
      i.unit_name = (const char*)query.getColumn(7);
      i.density = query.getColumn(8);
      return i;
    }

};

class Unit : public Model {
  public:
    string name;
    double value;
    bool is_weight;
};

void bindQuery(SQLite::Statement& query, const Unit& item);
void parseItem(SQLite::Statement& query, Unit& i);

class UnitTable : public Table<Unit> {
  public:
    const char* TABLE_NAME = "units";
    string getTableName() { return TABLE_NAME; };
    
    void bindQuery(SQLite::Statement& query, const Unit& item) {
      query.bind(1, item.name);
      query.bind(2, item.value);
      query.bind(3, item.is_weight);
      query.bind(4, item.created_at);
      query.bind(5, item.updated_at);
    }
    
    Unit parseItem(SQLite::Statement& query) {
      Unit i;
      i.name = (const char*)query.getColumn(1);
      i.value = query.getColumn(2);
      i.is_weight = (int)query.getColumn(3);
      i.created_at = query.getColumn(4);
      i.updated_at = query.getColumn(5);
      return i;
    }

};

class Recipe : public Model {
  public:
    string name;
    string instructions;
    double rating;
};

void bindQuery(SQLite::Statement& query, const Recipe& item);
void parseItem(SQLite::Statement& query, Recipe& i);

class RecipeTable : public Table<Recipe> {
  public:
    const char* TABLE_NAME = "recettes";
    string getTableName() { return TABLE_NAME; };
    
    void bindQuery(SQLite::Statement& query, const Recipe& item) {
      query.bind(1, item.name);
      query.bind(2, item.instructions);
      query.bind(3, item.rating);
      query.bind(4, item.created_at);
      query.bind(5, item.updated_at);
    }
    
    Recipe parseItem(SQLite::Statement& query) {
      Recipe i;
      i.name = (const char*)query.getColumn(1);
      i.instructions = (const char*)query.getColumn(2);
      i.rating = query.getColumn(3);
      i.created_at = query.getColumn(4);
      i.updated_at = query.getColumn(5);
      return i;
    }

};

class IngredientQuantity : public Model {
  public:
    int recipe_id;
    int ingredient_id;
    double value;
    int unit_id;
};

void bindQuery(SQLite::Statement& query, const IngredientQuantity& item);
void parseItem(SQLite::Statement& query, IngredientQuantity& i);

class IngredientQuantityTable : public Table<IngredientQuantity> {
  public:
    const char* TABLE_NAME = "ingredient_quantities";
    string getTableName() { return TABLE_NAME; };
    
    void bindQuery(SQLite::Statement& query, const IngredientQuantity& item) {
      query.bind(1, item.recipe_id);
      query.bind(2, item.ingredient_id);
      query.bind(3, item.value);
      query.bind(4, item.created_at);
      query.bind(5, item.updated_at);
      query.bind(6, item.unit_id);
    }
    
    IngredientQuantity parseItem(SQLite::Statement& query) {
      IngredientQuantity i;
      i.recipe_id = query.getColumn(1);
      i.ingredient_id = query.getColumn(2);
      i.value = query.getColumn(3);
      i.created_at = query.getColumn(4);
      i.updated_at = query.getColumn(5);
      i.unit_id = query.getColumn(6);
      return i;
    }
};

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

class ShelfTable : public Table<Shelf> {
  public:
    const char* TABLE_NAME = "shelves";
    string getTableName() { return TABLE_NAME; };
    
    void bindQuery(SQLite::Statement& query, const Shelf& item) {
      query.bind(1, item.height);
      query.bind(2, item.width);
      query.bind(3, item.depth);
      query.bind(6, item.created_at);
      query.bind(7, item.updated_at);
      query.bind(8, item.moving_height);
    }
    
    Shelf parseItem(SQLite::Statement& query) {
      Shelf i;
      i.height = query.getColumn(1);
      i.width = query.getColumn(2);
      i.depth = query.getColumn(3);
      i.created_at = query.getColumn(6);
      i.updated_at = query.getColumn(7);
      i.moving_height = query.getColumn(8);
      return i;
    }

};

//class Colonne : public Model {
//  public:
//    string name;
//    int shelf_id;
//    double width;
//    double center_x;
//};
//
//class ColonneTable : public Table<Colonne> {
//  public:
//    const char* TABLE_NAME = "colonnes";
//    string getTableName() { return TABLE_NAME; };
//    
//    void bindQuery(SQLite::Statement& query, const Colonne& item) {
//      query.bind(1, item.name);
//      query.bind(2, item.shelf_id);
//      query.bind(3, item.width);
//      query.bind(4, item.center_x);
//      query.bind(5, item.created_at);
//      query.bind(6, item.updated_at);
//    }
//    
//    Colonne parseItem(SQLite::Statement& query) {
//      Colonne i;
//      i.name = (const char*) query.getColumn(1);
//      i.shelf_id = query.getColumn(2);
//      i.width = query.getColumn(3);
//      i.center_x = query.getColumn(4);
//      i.created_at = query.getColumn(5);
//      i.updated_at = query.getColumn(6);
//      return i;
//    }
//
//};

//bool shelfHeightCmp(const Shelf& arg0, const Shelf& arg1);


#endif
