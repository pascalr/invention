class CreateRecipeTags < ActiveRecord::Migration[6.0]
  def change
    create_table :recipe_tags do |t|
      t.references :recette, null: false, foreign_key: true
      t.references :tag, null: false, foreign_key: true

      t.timestamps
    end
  end
end
