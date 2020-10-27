class ReorderColumnsDetected < ActiveRecord::Migration[6.0]
  def change
    change_column :detected_codes, :img, :string, after: :scale
  end
end
