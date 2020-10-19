class AddGripGapToHedas < ActiveRecord::Migration[6.0]
  def change
    add_column :hedas, :grip_gap, :float
  end
end
