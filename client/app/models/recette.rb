class Recette < ApplicationRecord
  belongs_to :recipe_category, optional: true
  has_many :ingredient_quantities
  has_many :recipe_tags
  has_many :tags, through: :recipe_tags

  def cost
    ingredient_quantities.inject(0){|sum,qty| sum + qty.cost }
  end

  def weight
    ingredient_quantities.inject(0){|sum,qty| sum + qty.weight }
  end

end
