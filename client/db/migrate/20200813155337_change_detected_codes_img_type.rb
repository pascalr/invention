class ChangeDetectedCodesImgType < ActiveRecord::Migration[6.0]
  def change
    remove_column :detected_codes, :img, :binary
    add_column :detected_codes, :img, :string
  end
end
