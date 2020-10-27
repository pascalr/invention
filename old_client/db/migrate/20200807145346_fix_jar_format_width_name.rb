class FixJarFormatWidthName < ActiveRecord::Migration[6.0]
  def change
    rename_column :jar_formats, :width, :diameter
  end
end
