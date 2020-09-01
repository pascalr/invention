class RecipeCategory < ApplicationRecord
  belongs_to :recipe_category, optional: true
  has_many :recipe_categories
end
