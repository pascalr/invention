class Shelf < ApplicationRecord
  has_many :locations
  has_many :jars, through: :locations
  has_many :ingredients, through: :jars
end
