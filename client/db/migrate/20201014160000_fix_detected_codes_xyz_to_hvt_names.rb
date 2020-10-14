class FixDetectedCodesXyzToHvtNames < ActiveRecord::Migration[6.0]
  def change
    rename_column :detected_codes, :x, :h
    rename_column :detected_codes, :y, :v
  end
end
