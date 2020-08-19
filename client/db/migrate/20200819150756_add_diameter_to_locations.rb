class AddDiameterToLocations < ActiveRecord::Migration[6.0]
  def change
    add_column :locations, :diameter, :float
  end
end
