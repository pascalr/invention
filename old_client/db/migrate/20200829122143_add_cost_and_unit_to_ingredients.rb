class AddCostAndUnitToIngredients < ActiveRecord::Migration[6.0]
  def change
    add_column :ingredients, :cost, :float
    add_column :ingredients, :quantity, :float
    add_column :ingredients, :unit_name, :string
  end
end
