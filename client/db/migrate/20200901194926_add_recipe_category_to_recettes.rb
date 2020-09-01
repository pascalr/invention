class AddRecipeCategoryToRecettes < ActiveRecord::Migration[6.0]
  def change
    add_column :recettes, :recipe_category_id, :integer
  end
end
