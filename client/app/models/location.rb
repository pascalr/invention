class Location < ApplicationRecord
  belongs_to :shelf
  belongs_to :column
  has_one :jar
end
