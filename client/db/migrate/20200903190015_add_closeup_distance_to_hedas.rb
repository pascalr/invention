class AddCloseupDistanceToHedas < ActiveRecord::Migration[6.0]
  def change
    add_column :hedas, :closeup_distance, :float
  end
end
