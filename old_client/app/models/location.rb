class Location < ApplicationRecord
  belongs_to :shelf
  belongs_to :column
  belongs_to :jar_format
  belongs_to :jar
end
