class AddJarIdToJars < ActiveRecord::Migration[6.0]
  def change
    add_column :jars, :jar_id, :integer
  end
end
