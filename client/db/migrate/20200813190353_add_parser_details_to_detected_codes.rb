class AddParserDetailsToDetectedCodes < ActiveRecord::Migration[6.0]
  def change
    add_column :detected_codes, :jar_id, :string
    add_column :detected_codes, :weight, :string
    add_column :detected_codes, :content_name, :string
    add_column :detected_codes, :content_id, :string
  end
end
