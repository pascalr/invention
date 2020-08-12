class Jar < ApplicationRecord
  belongs_to :jar_format
  belongs_to :ingredient
  belongs_to :position
end
