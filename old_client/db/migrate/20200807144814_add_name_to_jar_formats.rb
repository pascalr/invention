class AddNameToJarFormats < ActiveRecord::Migration[6.0]
  def change
    add_column :jar_formats, :name, :string
  end
end
