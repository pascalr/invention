class AddMaxPolarToHedas < ActiveRecord::Migration[6.0]
  def change
    add_column :hedas, :max_h, :float
    add_column :hedas, :max_v, :float
    add_column :hedas, :max_t, :float
  end
end
