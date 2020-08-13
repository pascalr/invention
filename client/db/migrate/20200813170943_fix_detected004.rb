class FixDetected004 < ActiveRecord::Migration[6.0]
  def change
    remove_column :detected_codes, :created_at, :datetime
    remove_column :detected_codes, :updated_at, :datetime
    remove_column :detected_codes, :img, :string
  end
end
