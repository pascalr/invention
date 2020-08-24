class CreateFdcs < ActiveRecord::Migration[6.0]
  def change
    create_table :fdcs do |t|
      t.string :data_type
      t.string :description
      t.integer :food_category_id
      t.string :publication_date

      t.timestamps
    end
  end
end
