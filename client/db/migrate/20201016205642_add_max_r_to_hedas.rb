class AddMaxRToHedas < ActiveRecord::Migration[6.0]
  def change
    add_column :hedas, :max_r, :float
  end
end
