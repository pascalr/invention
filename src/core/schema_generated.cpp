#include "schema_generated.h"

#include <string>

template <> std::string getTableName<HedaConfig>() { return "hedas"; }
template <> std::string getTableName<Spoon>() { return "spoons"; }
template <> std::string getTableName<Text>() { return "texts"; }
template <> std::string getTableName<Shelf>() { return "shelves"; }
template <> std::string getTableName<Location>() { return "locations"; }
template <> std::string getTableName<RecipeQuantity>() { return "recipe_quantities"; }
template <> std::string getTableName<Tag>() { return "tags"; }
template <> std::string getTableName<Faq>() { return "faqs"; }
template <> std::string getTableName<Jar>() { return "jars"; }
template <> std::string getTableName<Item>() { return "items"; }
template <> std::string getTableName<Unit>() { return "units"; }
template <> std::string getTableName<Ingredient>() { return "ingredients"; }
template <> std::string getTableName<TagOrder>() { return "tag_orders"; }
template <> std::string getTableName<RecipeTag>() { return "recipe_tags"; }
template <> std::string getTableName<IngredientQuantity>() { return "ingredient_quantities"; }
template <> std::string getTableName<DetectedHRCode>() { return "detected_codes"; }
template <> std::string getTableName<JarFormat>() { return "jar_formats"; }
template <> std::string getTableName<Image>() { return "images"; }
template <> std::string getTableName<Recipe>() { return "recipes"; }
template <> std::string getTableName<Meal>() { return "meals"; }

void parseItem(SQLite::Statement& query, HedaConfig& i) {
  i.working_shelf_id = query.getColumn(1);
  i.created_at = query.getColumn(2);
  i.updated_at = query.getColumn(3);
  i.user_coord_offset_x = query.getColumn(4);
  i.user_coord_offset_y = query.getColumn(5);
  i.user_coord_offset_z = query.getColumn(6);
  i.camera_radius = query.getColumn(7);
  i.gripper_radius = query.getColumn(8);
  i.camera_focal_point = query.getColumn(9);
  i.detect_height = query.getColumn(10);
  i.home_position_x = query.getColumn(11);
  i.home_position_y = query.getColumn(12);
  i.home_position_t = query.getColumn(13);
  i.grip_offset = query.getColumn(14);
  i.max_h = query.getColumn(15);
  i.max_v = query.getColumn(16);
  i.max_t = query.getColumn(17);
  i.closeup_distance = query.getColumn(18);
  i.max_x = query.getColumn(19);
  i.max_y = query.getColumn(20);
  i.max_z = query.getColumn(21);
  i.camera_width = query.getColumn(22);
  i.camera_height = query.getColumn(23);
  i.camera_calibration_height = query.getColumn(24);
  i.space_between_jaws = query.getColumn(25);
  i.grip_gap = query.getColumn(26);
  i.max_r = query.getColumn(27);
  i.working_x = query.getColumn(28);
  i.working_z = query.getColumn(29);
  i.bowl_x = query.getColumn(30);
  i.bowl_z = query.getColumn(31);
  i.scale_ratio = query.getColumn(32);
}

void parseItem(SQLite::Statement& query, Spoon& i) {
  i.offset_y = query.getColumn(1);
  i.radius = query.getColumn(2);
  i.volume = query.getColumn(3);
  i.created_at = query.getColumn(4);
  i.updated_at = query.getColumn(5);
}

void parseItem(SQLite::Statement& query, Text& i) {
  i.content = (const char*)query.getColumn(1);
  i.locale_name = (const char*)query.getColumn(2);
  i.created_at = query.getColumn(3);
  i.updated_at = query.getColumn(4);
  i.textable_id = query.getColumn(5);
  i.textable_type = (const char*)query.getColumn(6);
}

void parseItem(SQLite::Statement& query, Shelf& i) {
  i.height = query.getColumn(1);
  i.width = query.getColumn(2);
  i.depth = query.getColumn(3);
  i.offset_x = query.getColumn(4);
  i.offset_z = query.getColumn(5);
  i.created_at = query.getColumn(6);
  i.updated_at = query.getColumn(7);
  i.moving_height = query.getColumn(8);
}

void parseItem(SQLite::Statement& query, Location& i) {
  i.x = query.getColumn(1);
  i.z = query.getColumn(2);
  i.move_command = (const char*)query.getColumn(3);
  i.shelf_id = query.getColumn(4);
  i.created_at = query.getColumn(5);
  i.updated_at = query.getColumn(6);
  i.diameter = query.getColumn(7);
  i.is_storage = (int)query.getColumn(8);
  i.name = (const char*)query.getColumn(9);
  i.colonne_id = query.getColumn(10);
  i.jar_format_id = query.getColumn(11);
  i.jar_id = query.getColumn(12);
  i.occupied = (int)query.getColumn(13);
}

void parseItem(SQLite::Statement& query, RecipeQuantity& i) {
  i.recipe_id = query.getColumn(1);
  i.component_id = query.getColumn(2);
  i.value = query.getColumn(3);
  i.unit_id = query.getColumn(4);
  i.created_at = query.getColumn(5);
  i.updated_at = query.getColumn(6);
}

void parseItem(SQLite::Statement& query, Tag& i) {
  i.name = (const char*)query.getColumn(1);
  i.created_at = query.getColumn(2);
  i.updated_at = query.getColumn(3);
  i.priority = query.getColumn(4);
}

void parseItem(SQLite::Statement& query, Faq& i) {
  i.title = (const char*)query.getColumn(1);
  i.content = (const char*)query.getColumn(2);
  i.created_at = query.getColumn(3);
  i.updated_at = query.getColumn(4);
}

void parseItem(SQLite::Statement& query, Jar& i) {
  i.jar_format_id = query.getColumn(1);
  i.ingredient_id = query.getColumn(2);
  i.created_at = query.getColumn(3);
  i.updated_at = query.getColumn(4);
  i.location_id = query.getColumn(5);
  i.jar_id = query.getColumn(6);
}

void parseItem(SQLite::Statement& query, Item& i) {
  i.name = (const char*)query.getColumn(1);
  i.created_at = query.getColumn(2);
  i.updated_at = query.getColumn(3);
  i.default_image_id = query.getColumn(4);
  i.parent_id = query.getColumn(5);
}

void parseItem(SQLite::Statement& query, Unit& i) {
  i.name = (const char*)query.getColumn(1);
  i.value = query.getColumn(2);
  i.is_weight = (int)query.getColumn(3);
  i.created_at = query.getColumn(4);
  i.updated_at = query.getColumn(5);
  i.is_volume = (int)query.getColumn(6);
  i.show_fraction = (int)query.getColumn(7);
}

void parseItem(SQLite::Statement& query, Ingredient& i) {
  i.name = (const char*)query.getColumn(1);
  i.aliment_id = query.getColumn(2);
  i.created_at = query.getColumn(3);
  i.updated_at = query.getColumn(4);
  i.cost = query.getColumn(5);
  i.quantity = query.getColumn(6);
  i.density = query.getColumn(7);
  i.is_external = (int)query.getColumn(8);
  i.code = (const char*)query.getColumn(9);
  i.unit_id = query.getColumn(10);
  i.unit_weight = query.getColumn(11);
  i.unit_volume = query.getColumn(12);
}

void parseItem(SQLite::Statement& query, TagOrder& i) {
  i.tag_id = query.getColumn(1);
  i.priority = query.getColumn(2);
  i.created_at = query.getColumn(3);
  i.updated_at = query.getColumn(4);
}

void parseItem(SQLite::Statement& query, RecipeTag& i) {
  i.recipe_id = query.getColumn(1);
  i.tag_id = query.getColumn(2);
  i.created_at = query.getColumn(3);
  i.updated_at = query.getColumn(4);
}

void parseItem(SQLite::Statement& query, IngredientQuantity& i) {
  i.recipe_id = query.getColumn(1);
  i.ingredient_id = query.getColumn(2);
  i.value = query.getColumn(3);
  i.created_at = query.getColumn(4);
  i.updated_at = query.getColumn(5);
  i.unit_id = query.getColumn(6);
}

void parseItem(SQLite::Statement& query, DetectedHRCode& i) {
  i.h = query.getColumn(1);
  i.v = query.getColumn(2);
  i.t = query.getColumn(3);
  i.centerX = query.getColumn(4);
  i.centerY = query.getColumn(5);
  i.scale = query.getColumn(6);
  i.img = (const char*)query.getColumn(7);
  i.created_at = query.getColumn(8);
  i.updated_at = query.getColumn(9);
  i.jar_id = (const char*)query.getColumn(10);
  i.weight = (const char*)query.getColumn(11);
  i.content_name = (const char*)query.getColumn(12);
  i.content_id = (const char*)query.getColumn(13);
  i.lid_x = query.getColumn(14);
  i.lid_y = query.getColumn(15);
  i.lid_z = query.getColumn(16);
}

void parseItem(SQLite::Statement& query, JarFormat& i) {
  i.empty_weight = query.getColumn(1);
  i.height = query.getColumn(2);
  i.diameter = query.getColumn(3);
  i.created_at = query.getColumn(4);
  i.updated_at = query.getColumn(5);
  i.name = (const char*)query.getColumn(6);
  i.lid_diameter = query.getColumn(7);
  i.lid_weight = query.getColumn(8);
  i.grip_force = query.getColumn(9);
}

void parseItem(SQLite::Statement& query, Image& i) {
  i.width = query.getColumn(1);
  i.created_at = query.getColumn(2);
  i.updated_at = query.getColumn(3);
}

void parseItem(SQLite::Statement& query, Recipe& i) {
  i.name = (const char*)query.getColumn(1);
  i.instructions = (const char*)query.getColumn(2);
  i.rating = query.getColumn(3);
  i.created_at = query.getColumn(4);
  i.updated_at = query.getColumn(5);
  i.image_width = query.getColumn(6);
  i.source = (const char*)query.getColumn(7);
  i.primary_image_id = query.getColumn(8);
  i.secondary_image_left_id = query.getColumn(9);
  i.secondary_image_middle_id = query.getColumn(10);
  i.secondary_image_right_id = query.getColumn(11);
  i.item_id = query.getColumn(12);
  i.image_id = query.getColumn(13);
}

void parseItem(SQLite::Statement& query, Meal& i) {
  i.recipe_id = query.getColumn(1);
  i.start_time = query.getColumn(2);
  i.end_time = query.getColumn(3);
  i.created_at = query.getColumn(4);
  i.updated_at = query.getColumn(5);
}

