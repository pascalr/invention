class AllowNullAlimentIdForIngredient2 < ActiveRecord::Migration[6.0]
  def change
    change_column_null :ingredients, :aliment_id, true
    change_column_default :ingredients, :aliment_id, from: true, to: nil
  end
end
