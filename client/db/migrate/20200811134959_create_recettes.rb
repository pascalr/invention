class CreateRecettes < ActiveRecord::Migration[6.0]
  def change
    create_table :recettes do |t|
      t.string :name
      t.text :instructions
      t.float :rating

      t.timestamps
    end
  end
end
