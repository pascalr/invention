class AddIsExternalToIngredients < ActiveRecord::Migration[6.0]
  def change
    add_column :ingredients, :is_external, :boolean
  end
end
