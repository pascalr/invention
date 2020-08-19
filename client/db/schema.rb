# This file is auto-generated from the current state of the database. Instead
# of editing this file, please use the migrations feature of Active Record to
# incrementally modify your database, and then regenerate this schema definition.
#
# This file is the source Rails uses to define your schema when running `rails
# db:schema:load`. When creating a new database, `rails db:schema:load` tends to
# be faster and is potentially less error prone than running all of your
# migrations from scratch. Old migrations may fail to apply correctly if those
# migrations use external dependencies or application code.
#
# It's strongly recommended that you check this file into your version control system.

ActiveRecord::Schema.define(version: 2020_08_19_162928) do

  create_table "aliments", force: :cascade do |t|
    t.string "name"
    t.datetime "created_at", precision: 6, null: false
    t.datetime "updated_at", precision: 6, null: false
  end

  create_table "detected_codes", force: :cascade do |t|
    t.float "x"
    t.float "y"
    t.float "t"
    t.float "centerX"
    t.float "centerY"
    t.float "scale"
    t.string "img"
    t.datetime "created_at"
    t.datetime "updated_at"
    t.string "jar_id"
    t.string "weight"
    t.string "content_name"
    t.string "content_id"
    t.float "lid_x"
    t.float "lid_y"
    t.float "lid_z"
  end

  create_table "hedas", force: :cascade do |t|
    t.integer "shelf_id"
    t.datetime "created_at", precision: 6, null: false
    t.datetime "updated_at", precision: 6, null: false
    t.float "user_coord_offset_x"
    t.float "user_coord_offset_y"
    t.float "user_coord_offset_z"
    t.float "camera_radius"
    t.float "gripper_radius"
    t.float "camera_focal_point"
    t.float "detect_height"
    t.float "home_position_x"
    t.float "home_position_y"
    t.float "home_position_t"
    t.index ["shelf_id"], name: "index_hedas_on_shelf_id"
  end

  create_table "ingredients", force: :cascade do |t|
    t.string "name"
    t.integer "aliment_id"
    t.datetime "created_at", precision: 6, null: false
    t.datetime "updated_at", precision: 6, null: false
    t.index ["aliment_id"], name: "index_ingredients_on_aliment_id"
  end

  create_table "jar_formats", force: :cascade do |t|
    t.float "empty_weight"
    t.float "height"
    t.float "diameter"
    t.datetime "created_at", precision: 6, null: false
    t.datetime "updated_at", precision: 6, null: false
    t.string "name"
    t.float "lid_diameter"
    t.float "lid_weight"
  end

  create_table "jars", force: :cascade do |t|
    t.integer "jar_format_id"
    t.integer "ingredient_id"
    t.datetime "created_at", precision: 6, null: false
    t.datetime "updated_at", precision: 6, null: false
    t.integer "location_id"
  end

  create_table "locations", force: :cascade do |t|
    t.float "x"
    t.float "z"
    t.string "move_command"
    t.integer "shelf_id", null: false
    t.datetime "created_at", precision: 6, null: false
    t.datetime "updated_at", precision: 6, null: false
    t.float "diameter"
    t.index ["shelf_id"], name: "index_locations_on_shelf_id"
  end

  create_table "positions", force: :cascade do |t|
    t.float "x"
    t.float "y"
    t.float "t"
    t.datetime "created_at", precision: 6, null: false
    t.datetime "updated_at", precision: 6, null: false
  end

  create_table "recettes", force: :cascade do |t|
    t.string "name"
    t.text "instructions"
    t.float "rating"
    t.datetime "created_at", precision: 6, null: false
    t.datetime "updated_at", precision: 6, null: false
  end

  create_table "shelves", force: :cascade do |t|
    t.float "height"
    t.float "width"
    t.float "depth"
    t.float "offset_x"
    t.float "offset_z"
    t.datetime "created_at", precision: 6, null: false
    t.datetime "updated_at", precision: 6, null: false
    t.float "moving_height"
  end

  add_foreign_key "hedas", "shelves"
  add_foreign_key "ingredients", "aliments"
  add_foreign_key "locations", "shelves"
end
