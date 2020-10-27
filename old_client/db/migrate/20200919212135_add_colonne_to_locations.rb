class AddColonneToLocations < ActiveRecord::Migration[6.0]
  def change
    add_reference :locations, :colonne, null: true, foreign_key: true
  end
end
