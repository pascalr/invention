class CreateColonnes < ActiveRecord::Migration[6.0]
  def change
    create_table :colonnes do |t|
      t.string :name
      t.references :shelf, null: false, foreign_key: true
      t.float :width
      t.float :center_x

      t.timestamps
    end
  end
end
