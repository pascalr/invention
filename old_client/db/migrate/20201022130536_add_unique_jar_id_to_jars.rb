class AddUniqueJarIdToJars < ActiveRecord::Migration[6.0]
  def change
    add_index :jars, :jar_id, unique: true
  end
end
