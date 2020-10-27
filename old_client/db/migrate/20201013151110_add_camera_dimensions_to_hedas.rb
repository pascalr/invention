class AddCameraDimensionsToHedas < ActiveRecord::Migration[6.0]
  def change
    add_column :hedas, :camera_width, :float
    add_column :hedas, :camera_height, :float
  end
end
