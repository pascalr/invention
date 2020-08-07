class AddLidDiameterToJarFormats < ActiveRecord::Migration[6.0]
  def change
    add_column :jar_formats, :lid_diameter, :float
  end
end
