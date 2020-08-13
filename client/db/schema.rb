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

ActiveRecord::Schema.define(version: 2020_08_13_171032) do

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
    t.integer "jar_format_id", null: false
    t.integer "ingredient_id", null: false
    t.integer "position_id", null: false
    t.datetime "created_at", precision: 6, null: false
    t.datetime "updated_at", precision: 6, null: false
    t.index ["ingredient_id"], name: "index_jars_on_ingredient_id"
    t.index ["jar_format_id"], name: "index_jars_on_jar_format_id"
    t.index ["position_id"], name: "index_jars_on_position_id"
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
  end

  add_foreign_key "ingredients", "aliments"
  add_foreign_key "jars", "ingredients"
  add_foreign_key "jars", "jar_formats"
  add_foreign_key "jars", "positions"
end
