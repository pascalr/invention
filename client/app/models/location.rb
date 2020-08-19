class Location < ApplicationRecord
  belongs_to :shelf
  has_one :jar
end
