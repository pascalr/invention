class AddDetectHeightToHedas < ActiveRecord::Migration[6.0]
  def change
    add_column :hedas, :detect_height, :float
  end
end
