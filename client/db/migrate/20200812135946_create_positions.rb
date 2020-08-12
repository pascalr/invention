class CreatePositions < ActiveRecord::Migration[6.0]
  def change
    create_table :positions do |t|
      t.float :x
      t.float :y
      t.float :t

      t.timestamps
    end
  end
end
