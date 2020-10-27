class AddGripForceToJarFormats < ActiveRecord::Migration[6.0]
  def change
    add_column :jar_formats, :grip_force, :float
  end
end
