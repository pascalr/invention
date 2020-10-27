class CreateDetectedCodes < ActiveRecord::Migration[6.0]
  def change
    create_table :detected_codes do |t|
      t.float :x
      t.float :y
      t.float :t
      t.float :centerX
      t.float :centerY
      t.float :scale
      t.binary :img

      t.timestamps
    end
  end
end
