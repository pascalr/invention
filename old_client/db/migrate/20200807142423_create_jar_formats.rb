class CreateJarFormats < ActiveRecord::Migration[6.0]
  def change
    create_table :jar_formats do |t|
      t.float :empty_weight
      t.float :height
      t.float :width

      t.timestamps
    end
  end
end
