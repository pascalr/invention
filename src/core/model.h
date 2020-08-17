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
    //virtual string getValues(const T& item) = 0;
    virtual void bindQuery(SQLite::Statement& query, const T& item) = 0;
    //virtual void bindQuery(SQLite::Statement& query, const T& item, int i) = 0;

    //bool exists = false;

    int column_count;
    std::string update_query;
    std::string insert_query;
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

/*
0|id|integer|1||1
1|shelf_id|integer|0|NULL|0
2|created_at|datetime(6)|1||0
3|updated_at|datetime(6)|1||0
4|user_coord_offset_x|float|0|NULL|0
5|user_coord_offset_y|float|0|NULL|0
6|user_coord_offset_z|float|0|NULL|0
7|camera_radius|float|0|NULL|0
8|gripper_radius|float|0|NULL|0
9|camera_focal_point|float|0|NULL|0
*/

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
};

class HedaConfigTable : public Table<HedaConfig> {
  public:
    const char* TABLE_NAME = "hedas";
    string getTableName() { return TABLE_NAME; };

    HedaConfig parseItem(SQLite::Statement& query) {
      HedaConfig config;
      config.working_shelf_id = query.getColumn(0);
      config.created_at = query.getColumn(1);
      config.updated_at = query.getColumn(2);
      config.user_coord_offset_x = query.getColumn(3);
      config.user_coord_offset_y = query.getColumn(4);
      config.user_coord_offset_z = query.getColumn(5);
      config.camera_radius = query.getColumn(6);
      config.gripper_radius = query.getColumn(7);
      config.camera_focal_point = query.getColumn(8);
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
      query.bind(1, item.coord(0));
      query.bind(2, item.coord(1));
      query.bind(3, item.coord(2));
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
      query.bind(14, item.lid_coord(0));
      query.bind(15, item.lid_coord(1));
      query.bind(16, item.lid_coord(2));
    }

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
      code.lid_coord << query.getColumn(14), query.getColumn(15), query.getColumn(16);
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
    
    void bindQuery(SQLite::Statement& query, const Jar& item) {
      query.bind(1, item.position(0));
      query.bind(2, item.position(1));
      query.bind(3, item.position(2));
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
    int is_working_shelf; // deprecated, see HedaConfig.working_shelf_id
};

class ShelfTable : public Table<Shelf> {
  public:
    const char* TABLE_NAME = "shelves";
    string getTableName() { return TABLE_NAME; };
    
    void bindQuery(SQLite::Statement& query, const Shelf& item) {
      query.bind(1, item.height);
      query.bind(2, item.width);
      query.bind(3, item.depth);
      query.bind(4, item.is_working_shelf);
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

#endif
