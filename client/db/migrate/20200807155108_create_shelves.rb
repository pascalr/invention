class CreateShelves < ActiveRecord::Migration[6.0]
  def change
    create_table :shelves do |t|
      t.float :height
      t.float :width
      t.float :depth
      t.float :offset_x
      t.float :offset_z

      t.timestamps
    end
  end
end
