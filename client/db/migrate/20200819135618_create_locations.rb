class CreateLocations < ActiveRecord::Migration[6.0]
  def change
    create_table :locations do |t|
      t.float :x
      t.float :z
      t.string :move_command
      t.references :shelf, null: false, foreign_key: true

      t.timestamps
    end
  end
end
