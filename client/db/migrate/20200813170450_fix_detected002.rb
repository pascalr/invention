class FixDetected002 < ActiveRecord::Migration[6.0]
  def change
    remove_column :detected_codes, :img, :string
  end
end
