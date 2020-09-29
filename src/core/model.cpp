#include "model.h"

template <> string getTableName<DetectedHRCode>() { return "detected_codes"; }
template <> string getTableName<Jar>() { return "jars"; }
template <> string getTableName<Location>() { return "locations"; }
template <> string getTableName<Ingredient>() { return "ingredients"; }
template <> string getTableName<Shelf>() { return "shelves"; }
template <> string getTableName<HedaConfig>() { return "hedas"; }
template <> string getTableName<JarFormat>() { return "jar_formats"; }
template <> string getTableName<Unit>() { return "units"; }
template <> string getTableName<Recipe>() { return "recipes"; }
template <> string getTableName<IngredientQuantity>() { return "ingredient_quantities"; }

void bindQuery(SQLite::Statement& query, const IngredientQuantity& item) {
  query.bind(1, item.recipe_id);
  query.bind(2, item.ingredient_id);
  query.bind(3, item.value);
  query.bind(4, item.created_at);
  query.bind(5, item.updated_at);
  query.bind(6, item.unit_id);
}

void parseItem(SQLite::Statement& query, IngredientQuantity& i) {
  i.recipe_id = query.getColumn(1);
  i.ingredient_id = query.getColumn(2);
  i.value = query.getColumn(3);
  i.created_at = query.getColumn(4);
  i.updated_at = query.getColumn(5);
  i.unit_id = query.getColumn(6);
}

void bindQuery(SQLite::Statement& query, const Recipe& item) {
  query.bind(1, item.name);
  query.bind(2, item.instructions);
  query.bind(3, item.rating);
  query.bind(4, item.created_at);
  query.bind(5, item.updated_at);
}

void parseItem(SQLite::Statement& query, Recipe& i) {
  i.name = (const char*)query.getColumn(1);
  i.instructions = (const char*)query.getColumn(2);
  i.rating = query.getColumn(3);
  i.created_at = query.getColumn(4);
  i.updated_at = query.getColumn(5);
}

void bindQuery(SQLite::Statement& query, const Unit& item) {
  query.bind(1, item.name);
  query.bind(2, item.value);
  query.bind(3, item.is_weight);
  query.bind(4, item.created_at);
  query.bind(5, item.updated_at);
}

void parseItem(SQLite::Statement& query, Unit& i) {
  i.name = (const char*)query.getColumn(1);
  i.value = query.getColumn(2);
  i.is_weight = (int)query.getColumn(3);
  i.created_at = query.getColumn(4);
  i.updated_at = query.getColumn(5);
}

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

void parseItem(SQLite::Statement& query, JarFormat& item) {
  item.empty_weight = query.getColumn(1);
  item.height = query.getColumn(2);
  item.diameter = query.getColumn(3);
  item.created_at = query.getColumn(4);
  item.updated_at = query.getColumn(5);
  item.name = (const char*)query.getColumn(6);
  item.lid_diameter = query.getColumn(7);
  item.lid_weight = query.getColumn(8);
  item.grip_force = query.getColumn(9);
}

void parseItem(SQLite::Statement& query, HedaConfig& c) {
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

void parseItem(SQLite::Statement& query, Location& item) {
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

void bindQuery(SQLite::Statement& query, const Shelf& item) {
  query.bind(1, item.height);
  query.bind(2, item.width);
  query.bind(3, item.depth);
  query.bind(6, item.created_at);
  query.bind(7, item.updated_at);
  query.bind(8, item.moving_height);
}

void parseItem(SQLite::Statement& query, Shelf& i) {
  i.height = query.getColumn(1);
  i.width = query.getColumn(2);
  i.depth = query.getColumn(3);
  i.created_at = query.getColumn(6);
  i.updated_at = query.getColumn(7);
  i.moving_height = query.getColumn(8);
}

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

void parseItem(SQLite::Statement& query, Ingredient& i) {
  i.name = (const char*)query.getColumn(1);
  i.aliment_id = query.getColumn(2);
  i.created_at = query.getColumn(3);
  i.updated_at = query.getColumn(4);
  i.cost = query.getColumn(5);
  i.quantity = query.getColumn(6);
  i.unit_name = (const char*)query.getColumn(7);
  i.density = query.getColumn(8);
}

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

void parseItem(SQLite::Statement& query, DetectedHRCode& code) {
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
}
    
void bindQuery(SQLite::Statement& query, const Jar& item) {
  query.bind(1, item.jar_format_id);
  query.bind(2, item.ingredient_id);
  query.bind(3, item.created_at);
  query.bind(4, item.updated_at);
  query.bind(6, item.jar_id);
}

void parseItem(SQLite::Statement& query, Jar& jar) {
  jar.jar_format_id = query.getColumn(1);
  jar.ingredient_id = query.getColumn(2);
  jar.created_at = query.getColumn(3);
  jar.updated_at = query.getColumn(4);
  jar.jar_id = query.getColumn(6);
}

//bool shelfHeightCmp(const Shelf& arg0, const Shelf& arg1) {
//  return arg0.height < arg1.height;
//}
