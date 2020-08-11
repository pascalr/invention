class AllowNullAlimentIdForIngredient < ActiveRecord::Migration[6.0]
  def change
    change_column_default :ingredients, :aliment_id, from: false, to: true
  end
end
