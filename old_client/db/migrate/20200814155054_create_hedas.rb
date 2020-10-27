class CreateHedas < ActiveRecord::Migration[6.0]
  def change
    create_table :hedas do |t|
      t.references :shelf, null: false, foreign_key: true

      t.timestamps
    end
  end
end
