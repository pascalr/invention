class CreateIngredientQuantities < ActiveRecord::Migration[6.0]
  def change
    create_table :ingredient_quantities do |t|
      t.references :recette, null: false, foreign_key: true
      t.references :ingredient, null: false, foreign_key: true
      t.float :value

      t.timestamps
    end
  end
end
