class AddMaxXMaxYMaxZToHedas < ActiveRecord::Migration[6.0]
  def change
    add_column :hedas, :max_x, :float
    add_column :hedas, :max_y, :float
    add_column :hedas, :max_z, :float
  end
end
