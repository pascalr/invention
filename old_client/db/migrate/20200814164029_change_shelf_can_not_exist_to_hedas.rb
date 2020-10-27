class ChangeShelfCanNotExistToHedas < ActiveRecord::Migration[6.0]
  def change
    change_column_null :hedas, :shelf_id, true
  end
end
