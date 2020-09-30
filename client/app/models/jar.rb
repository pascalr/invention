class Jar < ApplicationRecord
  belongs_to :jar_format, optional: true
  belongs_to :ingredient, optional: true
  has_many :locations # has one storing location and one working shelf location
end
