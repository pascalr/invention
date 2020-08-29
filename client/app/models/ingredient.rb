class Ingredient < ApplicationRecord
  belongs_to :aliment, optional: true

  def unit
    Unit.find_by(name: unit_name)
  end
end
