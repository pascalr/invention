class AddIsStorageToLocations < ActiveRecord::Migration[6.0]
  def change
    add_column :locations, :is_storage, :boolean
  end
end
