#ifndef _SCHEMA_GENERATED_H
#define _SCHEMA_GENERATED_H

#include "model.h"

class DetectedHRCode : public Model {
  public:
    double h;
    double v;
    double t;
    double centerX;
    double centerY;
    double scale;
    std::string img;
    std::string jar_id;
    std::string weight;
    std::string content_name;
    std::string content_id;
    double lid_x;
    double lid_y;
    double lid_z;
};

class Faq : public Model {
  public:
    std::string title;
    std::string content;
};

class HedaConfig : public Model {
  public:
    int working_shelf_id;
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
    double camera_width;
    double camera_height;
    double camera_calibration_height;
    double space_between_jaws;
    double grip_gap;
    double max_r;
    double working_x;
    double working_z;
    double bowl_x;
    double bowl_z;
    double scale_ratio;
    double balance_x;
    double balance_z;
    double balance_offset;
};

class Image : public Model {
  public:
    double width;
};

class Ingredient : public Model {
  public:
    std::string name;
    int aliment_id;
    double cost;
    double quantity;
    double density;
    bool is_external;
    std::string code;
    int unit_id;
    double unit_weight;
    double unit_volume;
};

class IngredientQuantity : public Model {
  public:
    int recipe_id;
    int ingredient_id;
    double value;
    int unit_id;
};

class Item : public Model {
  public:
    std::string name;
    int default_image_id;
    int parent_id;
};

class Jar : public Model {
  public:
    int jar_format_id;
    int ingredient_id;
    int location_id;
    int jar_id;
    double weight;
};

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

class Location : public Model {
  public:
    double x;
    double z;
    std::string move_command;
    int shelf_id;
    double diameter;
    bool is_storage;
    std::string name;
    int colonne_id;
    int jar_format_id;
    int jar_id;
    bool occupied;
};

class Meal : public Model {
  public:
    int recipe_id;
    time_t start_time;
    time_t end_time;
};

class Recipe : public Model {
  public:
    std::string name;
    std::string instructions;
    double rating;
    double image_width;
    std::string source;
    int primary_image_id;
    int secondary_image_left_id;
    int secondary_image_middle_id;
    int secondary_image_right_id;
    int item_id;
    int image_id;
};

class RecipeQuantity : public Model {
  public:
    int recipe_id;
    int component_id;
    double value;
    int unit_id;
};

class RecipeTag : public Model {
  public:
    int recipe_id;
    int tag_id;
};

class Shelf : public Model {
  public:
    double height;
    double width;
    double depth;
    double offset_x;
    double offset_z;
    double moving_height;
};

class Spoon : public Model {
  public:
    double offset_y;
    double radius;
    double volume;
};

class Tag : public Model {
  public:
    std::string name;
    double priority;
};

class Text : public Model {
  public:
    std::string content;
    std::string locale_name;
    int textable_id;
    std::string textable_type;
};

class Unit : public Model {
  public:
    std::string name;
    double value;
    bool is_weight;
    bool is_volume;
    bool show_fraction;
};

template<typename T>
void bindQuery(T& query, const DetectedHRCode& item) {
  query.bind(1, item.h);
  query.bind(2, item.v);
  query.bind(3, item.t);
  query.bind(4, item.centerX);
  query.bind(5, item.centerY);
  query.bind(6, item.scale);
  query.bind(7, item.img);
  query.bind(8, item.created_at);
  query.bind(9, item.updated_at);
  query.bind(10, item.jar_id);
  query.bind(11, item.weight);
  query.bind(12, item.content_name);
  query.bind(13, item.content_id);
  query.bind(14, item.lid_x);
  query.bind(15, item.lid_y);
  query.bind(16, item.lid_z);
}

template<typename T>
void bindQuery(T& query, const Faq& item) {
  query.bind(1, item.title);
  query.bind(2, item.content);
  query.bind(3, item.created_at);
  query.bind(4, item.updated_at);
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
  query.bind(24, item.camera_calibration_height);
  query.bind(25, item.space_between_jaws);
  query.bind(26, item.grip_gap);
  query.bind(27, item.max_r);
  query.bind(28, item.working_x);
  query.bind(29, item.working_z);
  query.bind(30, item.bowl_x);
  query.bind(31, item.bowl_z);
  query.bind(32, item.scale_ratio);
  query.bind(33, item.balance_x);
  query.bind(34, item.balance_z);
  query.bind(35, item.balance_offset);
}

template<typename T>
void bindQuery(T& query, const Image& item) {
  query.bind(1, item.width);
  query.bind(2, item.created_at);
  query.bind(3, item.updated_at);
}

template<typename T>
void bindQuery(T& query, const Ingredient& item) {
  query.bind(1, item.name);
  query.bind(2, item.aliment_id);
  query.bind(3, item.created_at);
  query.bind(4, item.updated_at);
  query.bind(5, item.cost);
  query.bind(6, item.quantity);
  query.bind(7, item.density);
  query.bind(8, item.is_external);
  query.bind(9, item.code);
  query.bind(10, item.unit_id);
  query.bind(11, item.unit_weight);
  query.bind(12, item.unit_volume);
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
void bindQuery(T& query, const Item& item) {
  query.bind(1, item.name);
  query.bind(2, item.created_at);
  query.bind(3, item.updated_at);
  query.bind(4, item.default_image_id);
  query.bind(5, item.parent_id);
}

template<typename T>
void bindQuery(T& query, const Jar& item) {
  query.bind(1, item.jar_format_id);
  query.bind(2, item.ingredient_id);
  query.bind(3, item.created_at);
  query.bind(4, item.updated_at);
  query.bind(5, item.location_id);
  query.bind(6, item.jar_id);
  query.bind(7, item.weight);
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
  query.bind(10, item.colonne_id);
  query.bind(11, item.jar_format_id);
  query.bind(12, item.jar_id);
  query.bind(13, item.occupied);
}

template<typename T>
void bindQuery(T& query, const Meal& item) {
  query.bind(1, item.recipe_id);
  query.bind(2, item.start_time);
  query.bind(3, item.end_time);
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
  query.bind(6, item.image_width);
  query.bind(7, item.source);
  query.bind(8, item.primary_image_id);
  query.bind(9, item.secondary_image_left_id);
  query.bind(10, item.secondary_image_middle_id);
  query.bind(11, item.secondary_image_right_id);
  query.bind(12, item.item_id);
  query.bind(13, item.image_id);
}

template<typename T>
void bindQuery(T& query, const RecipeQuantity& item) {
  query.bind(1, item.recipe_id);
  query.bind(2, item.component_id);
  query.bind(3, item.value);
  query.bind(4, item.unit_id);
  query.bind(5, item.created_at);
  query.bind(6, item.updated_at);
}

template<typename T>
void bindQuery(T& query, const RecipeTag& item) {
  query.bind(1, item.recipe_id);
  query.bind(2, item.tag_id);
  query.bind(3, item.created_at);
  query.bind(4, item.updated_at);
}

template<typename T>
void bindQuery(T& query, const Shelf& item) {
  query.bind(1, item.height);
  query.bind(2, item.width);
  query.bind(3, item.depth);
  query.bind(4, item.offset_x);
  query.bind(5, item.offset_z);
  query.bind(6, item.created_at);
  query.bind(7, item.updated_at);
  query.bind(8, item.moving_height);
}

template<typename T>
void bindQuery(T& query, const Spoon& item) {
  query.bind(1, item.offset_y);
  query.bind(2, item.radius);
  query.bind(3, item.volume);
  query.bind(4, item.created_at);
  query.bind(5, item.updated_at);
}

template<typename T>
void bindQuery(T& query, const Tag& item) {
  query.bind(1, item.name);
  query.bind(2, item.created_at);
  query.bind(3, item.updated_at);
  query.bind(4, item.priority);
}

template<typename T>
void bindQuery(T& query, const Text& item) {
  query.bind(1, item.content);
  query.bind(2, item.locale_name);
  query.bind(3, item.created_at);
  query.bind(4, item.updated_at);
  query.bind(5, item.textable_id);
  query.bind(6, item.textable_type);
}

template<typename T>
void bindQuery(T& query, const Unit& item) {
  query.bind(1, item.name);
  query.bind(2, item.value);
  query.bind(3, item.is_weight);
  query.bind(4, item.created_at);
  query.bind(5, item.updated_at);
  query.bind(6, item.is_volume);
  query.bind(7, item.show_fraction);
}

void parseItem(SQLite::Statement& query, DetectedHRCode& i);void parseItem(SQLite::Statement& query, Faq& i);void parseItem(SQLite::Statement& query, HedaConfig& i);void parseItem(SQLite::Statement& query, Image& i);void parseItem(SQLite::Statement& query, Ingredient& i);void parseItem(SQLite::Statement& query, IngredientQuantity& i);void parseItem(SQLite::Statement& query, Item& i);void parseItem(SQLite::Statement& query, Jar& i);void parseItem(SQLite::Statement& query, JarFormat& i);void parseItem(SQLite::Statement& query, Location& i);void parseItem(SQLite::Statement& query, Meal& i);void parseItem(SQLite::Statement& query, Recipe& i);void parseItem(SQLite::Statement& query, RecipeQuantity& i);void parseItem(SQLite::Statement& query, RecipeTag& i);void parseItem(SQLite::Statement& query, Shelf& i);void parseItem(SQLite::Statement& query, Spoon& i);void parseItem(SQLite::Statement& query, Tag& i);void parseItem(SQLite::Statement& query, Text& i);void parseItem(SQLite::Statement& query, Unit& i);
#endif
