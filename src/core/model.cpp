#include "model.h"

// const char * Statement::getColumnDeclaredType(const int aIndex) const

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

void parseItem(SQLite::Statement& query, IngredientQuantity& i) {
  i.recipe_id = query.getColumn(1);
  i.ingredient_id = query.getColumn(2);
  i.value = query.getColumn(3);
  i.created_at = query.getColumn(4);
  i.updated_at = query.getColumn(5);
  i.unit_id = query.getColumn(6);
}



void parseItem(SQLite::Statement& query, Recipe& i) {
  i.name = (const char*)query.getColumn(1);
  i.instructions = (const char*)query.getColumn(2);
  i.rating = query.getColumn(3);
  i.created_at = query.getColumn(4);
  i.updated_at = query.getColumn(5);
}


void parseItem(SQLite::Statement& query, Unit& i) {
  i.name = (const char*)query.getColumn(1);
  i.value = query.getColumn(2);
  i.is_weight = (int)query.getColumn(3);
  i.created_at = query.getColumn(4);
  i.updated_at = query.getColumn(5);
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


void parseItem(SQLite::Statement& query, Shelf& i) {
  i.height = query.getColumn(1);
  i.width = query.getColumn(2);
  i.depth = query.getColumn(3);
  i.created_at = query.getColumn(6);
  i.updated_at = query.getColumn(7);
  i.moving_height = query.getColumn(8);
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
