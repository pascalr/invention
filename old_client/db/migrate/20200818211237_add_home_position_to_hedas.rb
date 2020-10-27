class AddHomePositionToHedas < ActiveRecord::Migration[6.0]
  def change
    add_column :hedas, :home_position_x, :float
    add_column :hedas, :home_position_y, :float
    add_column :hedas, :home_position_t, :float
  end
end
