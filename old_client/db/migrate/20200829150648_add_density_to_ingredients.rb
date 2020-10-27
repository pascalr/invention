class AddDensityToIngredients < ActiveRecord::Migration[6.0]
  def change
    add_column :ingredients, :density, :float
  end
end
