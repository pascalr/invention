class CreateRecipeCategories < ActiveRecord::Migration[6.0]
  def change
    create_table :recipe_categories do |t|
      t.string :name
      t.integer :recipe_category_id

      t.timestamps
    end
  end
end
