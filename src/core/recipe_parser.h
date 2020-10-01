#ifndef _RECIPE_PARSER_H
#define _RECIPE_PARSER_H

void parseRecipe() {
}

//bool is_processing_recipe = false;
//Recipe recipe_being_process;
//
//void process(Heda& heda, Recipe& recipe) {
//
//  heda.db.deleteFrom<IngredientQuantity>("WHERE recipe_id = " + to_string(recipe.id));
//
//  is_processing_recipe = true;
//  recipe_being_process = recipe;
//  execute(recipe.instructions);
//  is_processing_recipe = false;
//}
//
//double convert(double val, Unit fromUnit, Unit toUnit, double density) {
//
//  if (fromUnit.is_weight != toUnit.is_weight) {
//    if (fromUnit.is_weight) {
//      return val * (fromUnit.value / density) / toUnit.value;
//    } else {
//      return val * fromUnit.value / (toUnit.value / density);
//    }
//  }
//  return val * fromUnit.value / toUnit.value;
//}
//
//
//m_commands["ajouter"] = [&](ParseResult tokens) {

//  ensure(is_processing_recipe, "Must be processing recipe, because actually doing it is not implemented yet");

//  double s = tokens.popScalaire();
//  string unitName = tokens.popNoun();
//  string ingredientName = tokens.popNoun();

//  Unit unit = heda.db.findBy<Unit>("name", unitName, "COLLATE NOCASE");
//  ensure(unit.exists(), "ajouter must have a valid unit name");

//  Ingredient ingredient = heda.db.findBy<Ingredient>("name", ingredientName, "COLLATE NOCASE");
//  ensure(ingredient.exists(), "ajouter must have a valid ingredient name");

//  Unit toUnit = heda.db.findBy<Unit>("name", ingredient.unit_name, "COLLATE NOCASE");
//  ensure(toUnit.exists(), "ingredient must have a valid unit");

//  IngredientQuantity qty;
//  qty.recipe_id = recipe_being_process.id;
//  qty.ingredient_id = ingredient.id;
//  qty.value = s;
//  //qty.value = convert(s, unit, toUnit, ingredient.density);
//  qty.unit_id = unit.id;
//  heda.db.insert(qty);
//};

#endif 
