class Jar < ApplicationRecord
  belongs_to :jar_format, optional: true
  belongs_to :ingredient, optional: true
end
