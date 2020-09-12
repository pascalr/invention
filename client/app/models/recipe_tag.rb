class RecipeTag < ApplicationRecord
  belongs_to :recette
  belongs_to :tag
end
