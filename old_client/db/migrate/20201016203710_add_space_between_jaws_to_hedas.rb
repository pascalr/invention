class AddSpaceBetweenJawsToHedas < ActiveRecord::Migration[6.0]
  def change
    add_column :hedas, :space_between_jaws, :float
  end
end
