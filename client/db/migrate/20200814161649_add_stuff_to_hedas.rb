class AddStuffToHedas < ActiveRecord::Migration[6.0]
  def change
    add_column :hedas, :user_coord_offset_x, :float
    add_column :hedas, :user_coord_offset_y, :float
    add_column :hedas, :user_coord_offset_z, :float
    add_column :hedas, :camera_radius, :float
    add_column :hedas, :gripper_radius, :float
    add_column :hedas, :camera_focal_point, :float
  end
end
