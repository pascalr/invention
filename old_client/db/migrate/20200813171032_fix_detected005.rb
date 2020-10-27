class FixDetected005 < ActiveRecord::Migration[6.0]
  def change
    add_column :detected_codes, :img, :string
    add_column :detected_codes, :created_at, :datetime
    add_column :detected_codes, :updated_at, :datetime
  end
end
