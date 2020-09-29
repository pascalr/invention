class AddJarToLocations < ActiveRecord::Migration[6.0]
  def change
    add_reference :locations, :jar, null: false, foreign_key: true
  end
end
