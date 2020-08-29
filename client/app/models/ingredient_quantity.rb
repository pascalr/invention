def convert(val, fromUnit, toUnit, density)
  if fromUnit.is_weight == toUnit.is_weight
    return val * fromUnit.value / toUnit.value
  end
  if fromUnit.is_weight
    return val * (fromUnit.value / density) / toUnit.value;
  end
  return val * fromUnit.value / (toUnit.value / density);
end

class IngredientQuantity < ApplicationRecord
  belongs_to :recette
  belongs_to :ingredient
  belongs_to :unit

  def cost
    qty = convert(value, unit, ingredient.unit, ingredient.density)
    return ingredient.cost * qty / ingredient.quantity
  end

  def weight
    return value * unit.value if unit.is_weight
    return 0 if not ingredient.density
    return value * unit.value * ingredient.density
  end

  def volume
    return value * unit.value if not unit.is_weight
    return 0 if not ingredient.density
    return (value * unit.value) / ingredient.density
  end

end
