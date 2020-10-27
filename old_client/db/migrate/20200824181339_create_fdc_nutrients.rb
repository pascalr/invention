class CreateFdcNutrients < ActiveRecord::Migration[6.0]
  def change
    create_table :fdc_nutrients do |t|
      t.references :fdc, null: false, foreign_key: true
      t.references :nutrient, null: false, foreign_key: true
      t.float :amount
      t.float :median

      t.timestamps
    end
  end
end
