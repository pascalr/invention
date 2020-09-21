class Location < ApplicationRecord
  belongs_to :shelf
  belongs_to :column
  belongs_to :jar_format
  has_one :jar
end
