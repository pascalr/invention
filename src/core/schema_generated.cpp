#include "schema_generated.h"

#include <string>

#include "database.h"

template <> std::string getTableName<Capture>() { return "captures"; }
template <> std::string getTableName<DetectedHRCode>() { return "detected_codes"; }
template <> std::string getTableName<Faq>() { return "faqs"; }
template <> std::string getTableName<HedaConfig>() { return "hedas"; }
template <> std::string getTableName<Image>() { return "images"; }
template <> std::string getTableName<Ingredient>() { return "ingredients"; }
template <> std::string getTableName<IngredientQuantity>() { return "ingredient_quantities"; }
template <> std::string getTableName<Item>() { return "items"; }
template <> std::string getTableName<Jar>() { return "jars"; }
template <> std::string getTableName<JarFormat>() { return "jar_formats"; }
template <> std::string getTableName<Location>() { return "locations"; }
template <> std::string getTableName<Master>() { return "masters"; }
template <> std::string getTableName<Meal>() { return "meals"; }
template <> std::string getTableName<Recipe>() { return "recipes"; }
template <> std::string getTableName<RecipeQuantity>() { return "recipe_quantities"; }
template <> std::string getTableName<RecipeTag>() { return "recipe_tags"; }
template <> std::string getTableName<Shelf>() { return "shelves"; }
template <> std::string getTableName<Spoon>() { return "spoons"; }
template <> std::string getTableName<Tag>() { return "tags"; }
template <> std::string getTableName<Text>() { return "texts"; }
template <> std::string getTableName<Unit>() { return "units"; }

void parseItem(SQLite::Statement& query, Capture& i) {
  i.filename = (const char*)query.getColumn(1);
  i.h = query.getColumn(2);
  i.v = query.getColumn(3);
  i.t = query.getColumn(4);
  i.created_at = query.getColumn(5);
  i.updated_at = query.getColumn(6);
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

void parseItem(SQLite::Statement& query, Faq& i) {
  i.title = (const char*)query.getColumn(1);
  i.content = (const char*)query.getColumn(2);
  i.created_at = query.getColumn(3);
  i.updated_at = query.getColumn(4);
}

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
  i.balance_x = query.getColumn(33);
  i.balance_z = query.getColumn(34);
  i.balance_offset = query.getColumn(35);
}

void parseItem(SQLite::Statement& query, Image& i) {
  i.width = query.getColumn(1);
  i.created_at = query.getColumn(2);
  i.updated_at = query.getColumn(3);
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

void parseItem(SQLite::Statement& query, IngredientQuantity& i) {
  i.recipe_id = query.getColumn(1);
  i.ingredient_id = query.getColumn(2);
  i.value = query.getColumn(3);
  i.created_at = query.getColumn(4);
  i.updated_at = query.getColumn(5);
  i.unit_id = query.getColumn(6);
}

void parseItem(SQLite::Statement& query, Item& i) {
  i.name = (const char*)query.getColumn(1);
  i.created_at = query.getColumn(2);
  i.updated_at = query.getColumn(3);
  i.default_image_id = query.getColumn(4);
  i.parent_id = query.getColumn(5);
}

void parseItem(SQLite::Statement& query, Jar& i) {
  i.jar_format_id = query.getColumn(1);
  i.ingredient_id = query.getColumn(2);
  i.created_at = query.getColumn(3);
  i.updated_at = query.getColumn(4);
  i.location_id = query.getColumn(5);
  i.jar_id = query.getColumn(6);
  i.weight = query.getColumn(7);
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

void parseItem(SQLite::Statement& query, Master& i) {
  i.h = query.getColumn(1);
  i.v = query.getColumn(2);
  i.t = query.getColumn(3);
  i.created_at = query.getColumn(4);
  i.updated_at = query.getColumn(5);
}

void parseItem(SQLite::Statement& query, Meal& i) {
  i.recipe_id = query.getColumn(1);
  i.start_time = query.getColumn(2);
  i.end_time = query.getColumn(3);
  i.created_at = query.getColumn(4);
  i.updated_at = query.getColumn(5);
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

void parseItem(SQLite::Statement& query, RecipeQuantity& i) {
  i.recipe_id = query.getColumn(1);
  i.component_id = query.getColumn(2);
  i.value = query.getColumn(3);
  i.unit_id = query.getColumn(4);
  i.created_at = query.getColumn(5);
  i.updated_at = query.getColumn(6);
}

void parseItem(SQLite::Statement& query, RecipeTag& i) {
  i.recipe_id = query.getColumn(1);
  i.tag_id = query.getColumn(2);
  i.created_at = query.getColumn(3);
  i.updated_at = query.getColumn(4);
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

void parseItem(SQLite::Statement& query, Spoon& i) {
  i.offset_y = query.getColumn(1);
  i.radius = query.getColumn(2);
  i.volume = query.getColumn(3);
  i.created_at = query.getColumn(4);
  i.updated_at = query.getColumn(5);
}

void parseItem(SQLite::Statement& query, Tag& i) {
  i.name = (const char*)query.getColumn(1);
  i.created_at = query.getColumn(2);
  i.updated_at = query.getColumn(3);
  i.priority = query.getColumn(4);
}

void parseItem(SQLite::Statement& query, Text& i) {
  i.content = (const char*)query.getColumn(1);
  i.locale_name = (const char*)query.getColumn(2);
  i.created_at = query.getColumn(3);
  i.updated_at = query.getColumn(4);
  i.textable_id = query.getColumn(5);
  i.textable_type = (const char*)query.getColumn(6);
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

Shelf HedaConfig::getWorkingShelf() {
  if (working_shelf == NULL) {
    Shelf __record = Db::conn().db.find<Shelf>(working_shelf_id);
    if (!__record.exists()) throw MissingAssociationException("Could not find HedaConfig.working_shelf with working_shelf_id=" + std::to_string(working_shelf_id));
    working_shelf = new Shelf(__record);
  }
  return *working_shelf;
}

Unit Ingredient::getUnit() {
  if (unit == NULL) {
    Unit __record = Db::conn().db.find<Unit>(unit_id);
  }
  return *unit;
}

Recipe IngredientQuantity::getRecipe() {
  if (recipe == NULL) {
    Recipe __record = Db::conn().db.find<Recipe>(recipe_id);
    if (!__record.exists()) throw MissingAssociationException("Could not find IngredientQuantity.recipe with recipe_id=" + std::to_string(recipe_id));
    recipe = new Recipe(__record);
  }
  return *recipe;
}

Ingredient IngredientQuantity::getIngredient() {
  if (ingredient == NULL) {
    Ingredient __record = Db::conn().db.find<Ingredient>(ingredient_id);
    if (!__record.exists()) throw MissingAssociationException("Could not find IngredientQuantity.ingredient with ingredient_id=" + std::to_string(ingredient_id));
    ingredient = new Ingredient(__record);
  }
  return *ingredient;
}

Unit IngredientQuantity::getUnit() {
  if (unit == NULL) {
    Unit __record = Db::conn().db.find<Unit>(unit_id);
  }
  return *unit;
}

Item Item::getParent() {
  if (parent == NULL) {
    Item __record = Db::conn().db.find<Item>(parent_id);
  }
  return *parent;
}

Image Item::getDefaultImage() {
  if (default_image == NULL) {
    Image __record = Db::conn().db.find<Image>(default_image_id);
  }
  return *default_image;
}

JarFormat Jar::getJarFormat() {
  if (jar_format == NULL) {
    JarFormat __record = Db::conn().db.find<JarFormat>(jar_format_id);
    if (!__record.exists()) throw MissingAssociationException("Could not find Jar.jar_format with jar_format_id=" + std::to_string(jar_format_id));
    jar_format = new JarFormat(__record);
  }
  return *jar_format;
}

Ingredient Jar::getIngredient() {
  if (ingredient == NULL) {
    Ingredient __record = Db::conn().db.find<Ingredient>(ingredient_id);
    if (!__record.exists()) throw MissingAssociationException("Could not find Jar.ingredient with ingredient_id=" + std::to_string(ingredient_id));
    ingredient = new Ingredient(__record);
  }
  return *ingredient;
}

JarFormat Location::getJarFormat() {
  if (jar_format == NULL) {
    JarFormat __record = Db::conn().db.find<JarFormat>(jar_format_id);
    if (!__record.exists()) throw MissingAssociationException("Could not find Location.jar_format with jar_format_id=" + std::to_string(jar_format_id));
    jar_format = new JarFormat(__record);
  }
  return *jar_format;
}

Jar Location::getJar() {
  if (jar == NULL) {
    Jar __record = Db::conn().db.find<Jar>(jar_id);
    if (!__record.exists()) throw MissingAssociationException("Could not find Location.jar with jar_id=" + std::to_string(jar_id));
    jar = new Jar(__record);
  }
  return *jar;
}

Shelf Location::getShelf() {
  if (shelf == NULL) {
    Shelf __record = Db::conn().db.find<Shelf>(shelf_id);
    if (!__record.exists()) throw MissingAssociationException("Could not find Location.shelf with shelf_id=" + std::to_string(shelf_id));
    shelf = new Shelf(__record);
  }
  return *shelf;
}

Recipe Meal::getRecipe() {
  if (recipe == NULL) {
    Recipe __record = Db::conn().db.find<Recipe>(recipe_id);
    if (!__record.exists()) throw MissingAssociationException("Could not find Meal.recipe with recipe_id=" + std::to_string(recipe_id));
    recipe = new Recipe(__record);
  }
  return *recipe;
}

Image Recipe::getImage() {
  if (image == NULL) {
    Image __record = Db::conn().db.find<Image>(image_id);
  }
  return *image;
}

Item Recipe::getItem() {
  if (item == NULL) {
    Item __record = Db::conn().db.find<Item>(item_id);
    if (!__record.exists()) throw MissingAssociationException("Could not find Recipe.item with item_id=" + std::to_string(item_id));
    item = new Item(__record);
  }
  return *item;
}

Image Recipe::getPrimaryImage() {
  if (primary_image == NULL) {
    Image __record = Db::conn().db.find<Image>(primary_image_id);
  }
  return *primary_image;
}

Image Recipe::getSecondaryImageLeft() {
  if (secondary_image_left == NULL) {
    Image __record = Db::conn().db.find<Image>(secondary_image_left_id);
  }
  return *secondary_image_left;
}

Image Recipe::getSecondaryImageMiddle() {
  if (secondary_image_middle == NULL) {
    Image __record = Db::conn().db.find<Image>(secondary_image_middle_id);
  }
  return *secondary_image_middle;
}

Image Recipe::getSecondaryImageRight() {
  if (secondary_image_right == NULL) {
    Image __record = Db::conn().db.find<Image>(secondary_image_right_id);
  }
  return *secondary_image_right;
}

Recipe RecipeQuantity::getRecipe() {
  if (recipe == NULL) {
    Recipe __record = Db::conn().db.find<Recipe>(recipe_id);
    if (!__record.exists()) throw MissingAssociationException("Could not find RecipeQuantity.recipe with recipe_id=" + std::to_string(recipe_id));
    recipe = new Recipe(__record);
  }
  return *recipe;
}

Recipe RecipeQuantity::getComponent() {
  if (component == NULL) {
    Recipe __record = Db::conn().db.find<Recipe>(component_id);
  }
  return *component;
}

Unit RecipeQuantity::getUnit() {
  if (unit == NULL) {
    Unit __record = Db::conn().db.find<Unit>(unit_id);
  }
  return *unit;
}

Recipe RecipeTag::getRecipe() {
  if (recipe == NULL) {
    Recipe __record = Db::conn().db.find<Recipe>(recipe_id);
    if (!__record.exists()) throw MissingAssociationException("Could not find RecipeTag.recipe with recipe_id=" + std::to_string(recipe_id));
    recipe = new Recipe(__record);
  }
  return *recipe;
}

Tag RecipeTag::getTag() {
  if (tag == NULL) {
    Tag __record = Db::conn().db.find<Tag>(tag_id);
    if (!__record.exists()) throw MissingAssociationException("Could not find RecipeTag.tag with tag_id=" + std::to_string(tag_id));
    tag = new Tag(__record);
  }
  return *tag;
}

Capture::~Capture() {
}

DetectedHRCode::~DetectedHRCode() {
}

Faq::~Faq() {
}

HedaConfig::~HedaConfig() {
  if (working_shelf != NULL) delete working_shelf;
}

Image::~Image() {
}

Ingredient::~Ingredient() {
  if (unit != NULL) delete unit;
}

IngredientQuantity::~IngredientQuantity() {
  if (recipe != NULL) delete recipe;
  if (ingredient != NULL) delete ingredient;
  if (unit != NULL) delete unit;
}

Item::~Item() {
  if (parent != NULL) delete parent;
  if (default_image != NULL) delete default_image;
}

Jar::~Jar() {
  if (jar_format != NULL) delete jar_format;
  if (ingredient != NULL) delete ingredient;
}

JarFormat::~JarFormat() {
}

Location::~Location() {
  if (jar_format != NULL) delete jar_format;
  if (jar != NULL) delete jar;
  if (shelf != NULL) delete shelf;
}

Master::~Master() {
}

Meal::~Meal() {
  if (recipe != NULL) delete recipe;
}

Recipe::~Recipe() {
  if (image != NULL) delete image;
  if (item != NULL) delete item;
  if (primary_image != NULL) delete primary_image;
  if (secondary_image_left != NULL) delete secondary_image_left;
  if (secondary_image_middle != NULL) delete secondary_image_middle;
  if (secondary_image_right != NULL) delete secondary_image_right;
}

RecipeQuantity::~RecipeQuantity() {
  if (recipe != NULL) delete recipe;
  if (component != NULL) delete component;
  if (unit != NULL) delete unit;
}

RecipeTag::~RecipeTag() {
  if (recipe != NULL) delete recipe;
  if (tag != NULL) delete tag;
}

Shelf::~Shelf() {
}

Spoon::~Spoon() {
}

Tag::~Tag() {
}

Text::~Text() {
}

Unit::~Unit() {
}

