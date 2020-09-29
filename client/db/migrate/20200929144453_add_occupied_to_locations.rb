class AddOccupiedToLocations < ActiveRecord::Migration[6.0]
  def change
    add_column :locations, :occupied, :boolean
  end
end
