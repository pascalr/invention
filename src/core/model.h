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

template<class T>
class Table {
  public:

    virtual string getTableName() = 0;
    virtual T parseItem(SQLite::Statement& query) = 0;
    //virtual string getValues(const T& item) = 0;
    virtual void bindQuery(SQLite::Statement& query, const T& item) = 0;
    //virtual void bindQuery(SQLite::Statement& query, const T& item, int i) = 0;
    int get(T& item, int id) {
      for (auto it = items.begin(); it != items.end(); ++it) {
        if (it->id == id) {
          item = *it;
          return 1;
        }
      }
      return 0;
    }

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
    void orderBy(bool (*cmp)(const T&, const T&), bool ascending=true) {
      if (ascending) {
        std::sort(items.begin(), items.end(), cmp);
      } else {
        std::sort(items.rbegin(), items.rend(), cmp);
      }
    }
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
};

class HedaConfigTable : public Table<HedaConfig> {
  public:
    const char* TABLE_NAME = "hedas";
    string getTableName() { return TABLE_NAME; };

    HedaConfig parseItem(SQLite::Statement& query) {
      HedaConfig config;
      config.working_shelf_id = query.getColumn(1);
      config.created_at = query.getColumn(2);
      config.updated_at = query.getColumn(3);
      config.user_coord_offset_x = query.getColumn(4);
      config.user_coord_offset_y = query.getColumn(5);
      config.user_coord_offset_z = query.getColumn(6);
      config.camera_radius = query.getColumn(7);
      config.gripper_radius = query.getColumn(8);
      config.camera_focal_point = query.getColumn(9);
      config.detect_height = query.getColumn(10);
      config.home_position_x = query.getColumn(11);
      config.home_position_y = query.getColumn(12);
      config.home_position_t = query.getColumn(13);
      return config;
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
};

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
    }
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
    UserCoord lid_coord;
};

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
};

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
      return item;
    }

};

class Jar : public Model {
  public:
    int jar_format_id = -1;
    int ingredient_id = -1;
    int location_id = -1;
    //PolarCoord position;
};

class JarTable : public Table<Jar> {
  public:
    const char* TABLE_NAME = "jars";
    string getTableName() { return TABLE_NAME; };
    
    void bindQuery(SQLite::Statement& query, const Jar& item) {
      query.bind(1, item.jar_format_id);
      query.bind(2, item.ingredient_id);
      query.bind(3, item.created_at);
      query.bind(4, item.updated_at);
      query.bind(5, item.location_id);
      //query.bind(1, item.position(0));
      //query.bind(2, item.position(1));
      //query.bind(3, item.position(2));
    }

    Jar parseItem(SQLite::Statement& query) {
      Jar jar;
      jar.jar_format_id = query.getColumn(1);
      jar.ingredient_id = query.getColumn(2);
      jar.created_at = query.getColumn(3);
      jar.updated_at = query.getColumn(4);
      jar.location_id = query.getColumn(5);
      //double x = query.getColumn(1);
      //double y = query.getColumn(2);
      //double t = query.getColumn(3);
      return jar;
    }

};

class Ingredient : public Model {
  public:
    string name;
    int aliment_id;
};

class IngredientTable : public Table<Ingredient> {
  public:
    const char* TABLE_NAME = "ingredients";
    string getTableName() { return TABLE_NAME; };
    
    void bindQuery(SQLite::Statement& query, const Ingredient& item) {
      query.bind(1, item.name);
      query.bind(2, item.aliment_id);
      query.bind(3, item.created_at);
      query.bind(4, item.updated_at);
    }
    
    Ingredient parseItem(SQLite::Statement& query) {
      Ingredient i;
      i.name = (const char*)query.getColumn(1);
      i.aliment_id = query.getColumn(2);
      i.created_at = query.getColumn(3);
      i.updated_at = query.getColumn(4);
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

bool shelfHeightCmp(const Shelf& arg0, const Shelf& arg1);

#endif
