class AddMovingHeightToShelves < ActiveRecord::Migration[6.0]
  def change
    add_column :shelves, :moving_height, :float
  end
end
