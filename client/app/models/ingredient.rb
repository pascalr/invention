class Ingredient < ApplicationRecord
  belongs_to :aliment, optional: true
end
