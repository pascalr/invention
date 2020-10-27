class FixDetected003 < ActiveRecord::Migration[6.0]
  def change
    add_column :detected_codes, :img, :string, after: :scale
  end
end
