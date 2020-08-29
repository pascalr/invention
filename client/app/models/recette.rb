class Recette < ApplicationRecord
  has_many :ingredient_quantities

  def cost
    ingredient_quantities.inject(0){|sum,qty| sum + qty.cost }
  end
end
