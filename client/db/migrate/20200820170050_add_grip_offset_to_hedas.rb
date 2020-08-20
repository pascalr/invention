class AddGripOffsetToHedas < ActiveRecord::Migration[6.0]
  def change
    add_column :hedas, :grip_offset, :float
  end
end
