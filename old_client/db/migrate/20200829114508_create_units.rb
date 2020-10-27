class CreateUnits < ActiveRecord::Migration[6.0]
  def change
    create_table :units do |t|
      t.string :name
      t.float :value
      t.boolean :is_weight

      t.timestamps
    end
  end
end
