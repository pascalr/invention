class CreateJars < ActiveRecord::Migration[6.0]
  def change
    create_table :jars do |t|
      t.references :jar_format, null: false, foreign_key: true
      t.references :ingredient, null: false, foreign_key: true
      t.references :position, null: false, foreign_key: true

      t.timestamps
    end
  end
end
