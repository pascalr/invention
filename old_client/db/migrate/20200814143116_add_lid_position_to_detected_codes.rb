class AddLidPositionToDetectedCodes < ActiveRecord::Migration[6.0]
  def change
    add_column :detected_codes, :lid_x, :float
    add_column :detected_codes, :lid_y, :float
    add_column :detected_codes, :lid_z, :float
  end
end
