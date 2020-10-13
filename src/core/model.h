#ifndef _MODEL_H
#define _MODEL_H

#include "SQLiteCpp/SQLiteCpp.h"
#include "position.h"
#include <vector>
#include <iostream>

#include "../lib/opencv.h"
#include "../lib/hr_code.h"

class WrongModelTypeException : public std::exception {};

// TODO: Validate column names indexes

template <class T>
int byJarId(const T& t) {
  return t.jar_id;
}
template <class T>
std::string byName(const T& t) {
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
    double camera_height;
    double camera_width;

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

class Location : public Model {
  public:
    Location() {}
    Location(double x, double z, std::string moveCommand, int shelfId, double diameter) : x(x), z(z), move_command(moveCommand), shelf_id(shelfId), diameter(diameter) {}

    double x;
    double z;
    std::string move_command;
    int shelf_id;

    double diameter;
    bool is_storage;
    std::string name;
    int jar_format_id;
    int jar_id;
    bool occupied;
};

void parseItem(SQLite::Statement& query, Location& item);

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
    std::string imgFilename;
    std::string jar_id;
    std::string weight;
    std::string content_name;
    std::string content_id;
    UserCoord lid_coord;
};

void parseItem(SQLite::Statement& query, DetectedHRCode& code);

template<typename T>
std::string getTableName();

class JarFormat : public Model {
  public:
    double empty_weight;
    double height;
    double diameter;
    std::string name;
    double lid_diameter;
    double lid_weight;
    double grip_force;
};

void parseItem(SQLite::Statement& query, JarFormat& item);

class Jar : public Model {
  public:
    int jar_format_id = -1;
    int ingredient_id = -1;
    int jar_id = -1;
};

void parseItem(SQLite::Statement& query, Jar& item);

class Ingredient : public Model {
  public:
    std::string name;
    int aliment_id;
    double cost;
    double quantity;
    std::string unit_name;
    double density;
};

void parseItem(SQLite::Statement& query, Ingredient& i);

class Unit : public Model {
  public:
    std::string name;
    double value;
    bool is_weight;
};

void parseItem(SQLite::Statement& query, Unit& i);

class Recipe : public Model {
  public:
    std::string name;
    std::string instructions;
    double rating;
};

void parseItem(SQLite::Statement& query, Recipe& i);

class IngredientQuantity : public Model {
  public:
    int recipe_id;
    int ingredient_id;
    double value;
    int unit_id;
};

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
    
void parseItem(SQLite::Statement& query, Shelf& i);

template<typename T>
void bindQuery(T& query, const Unit& item) {
  query.bind(1, item.name);
  query.bind(2, item.value);
  query.bind(3, item.is_weight);
  query.bind(4, item.created_at);
  query.bind(5, item.updated_at);
}
template<typename T>
void bindQuery(T& query, const Recipe& item) {
  query.bind(1, item.name);
  query.bind(2, item.instructions);
  query.bind(3, item.rating);
  query.bind(4, item.created_at);
  query.bind(5, item.updated_at);
}
template<typename T>
void bindQuery(T& query, const IngredientQuantity& item) {
  query.bind(1, item.recipe_id);
  query.bind(2, item.ingredient_id);
  query.bind(3, item.value);
  query.bind(4, item.created_at);
  query.bind(5, item.updated_at);
  query.bind(6, item.unit_id);
}
template<typename T>
void bindQuery(T& query, const JarFormat& item) {
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
template<typename T>
void bindQuery(T& query, const HedaConfig& item) {
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
  query.bind(22, item.camera_width);
  query.bind(23, item.camera_height);
}
template<typename T>
void bindQuery(T& query, const Location& item) {
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
template<typename T>
void bindQuery(T& query, const Shelf& item) {
  query.bind(1, item.height);
  query.bind(2, item.width);
  query.bind(3, item.depth);
  query.bind(6, item.created_at);
  query.bind(7, item.updated_at);
  query.bind(8, item.moving_height);
}
template<typename T>
void bindQuery(T& query, const Ingredient& item) {
  query.bind(1, item.name);
  query.bind(2, item.aliment_id);
  query.bind(3, item.created_at);
  query.bind(4, item.updated_at);
  query.bind(5, item.cost);
  query.bind(6, item.quantity);
  query.bind(7, item.unit_name);
  query.bind(8, item.density);
}
template<typename T>
void bindQuery(T& query, const DetectedHRCode& item) {
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
template<typename T>
void bindQuery(T& query, const Jar& item) {
  query.bind(1, item.jar_format_id);
  query.bind(2, item.ingredient_id);
  query.bind(3, item.created_at);
  query.bind(4, item.updated_at);
  query.bind(6, item.jar_id);
}

#endif
