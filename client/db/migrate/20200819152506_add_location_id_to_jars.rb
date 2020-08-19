class AddLocationIdToJars < ActiveRecord::Migration[6.0]
  def change
    add_column :jars, :location_id, :integer
  end
end
