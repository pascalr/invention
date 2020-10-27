class CreateJars < ActiveRecord::Migration[6.0]
  def change
    create_table :jars do |t|
      t.integer :jar_format_id
      t.integer :ingredient_id

      t.timestamps
    end
  end
end
