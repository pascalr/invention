class AddCodeToIngredients < ActiveRecord::Migration[6.0]
  def change
    add_column :ingredients, :code, :string
  end
end
