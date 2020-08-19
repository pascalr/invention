class Jar < ApplicationRecord
  belongs_to :jar_format, optional: true
  belongs_to :ingredient, optional: true
  belongs_to :location, optional: true
end
