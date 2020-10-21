#ifndef _RECIPE_PARSER_H
#define _RECIPE_PARSER_H

#include "model.h"
#include "database.h"
#include "log.h"

// Returns the substring until the next space. Updates the str parse what has been extracted.
std::string parseWord(std::string& str) {
 
  ltrim(str);
  std::string cmd = str.substr(0, str.find(' '));
  str = str.substr(cmd.length());
  return cmd;
}

// TODO: Later handle escape character, either a backslash at the end or inside quotes it's not over by newline.
std::string parseSentence(std::string& str) {
 
  std::string cmd = str.substr(0, str.find('\n'));
  str = str.substr(cmd.length());
  trim(cmd);
  return cmd;
}

double parseFloat(std::string& str) {
  
  std::string number = parseWord(str);

  char* pEnd;
  double val = strtod(number.c_str(), &pEnd);
  ensure(strlen(pEnd) == 0, "Was unable the parse a number with word = " + number);
  return val;
}

template<typename T>
T parseDb(Database& db, std::string& str, std::string columnName) {
  
  ltrim(str);

  int maxLength = std::max(str.length(), db.getMaxLength<T>(columnName));

  for (int i = maxLength; i > 0; i--) {
    std::string sub = str.substr(0,i-1);
    T item = db.findBy<T>(columnName, sub, "COLLATE NOCASE");
    if (item.exists()) {
      str = str.substr(sub.length());
      return item;
    }
  }

  T dontExist; return dontExist;
}

template<typename T>
T mustParseDb(Database& db, std::string& str, std::string columnName) {

  T result = parseDb<T>(db, str, columnName);  
  ensure(result.exists(), "Was unable to parse a unit with sentence = " + str);
  return result;
}


//select top 1 CR
//from table t
//order by len(CR) desc

void ajouter(Database& db, Recipe& recipe, double quantity, Unit& unit, Ingredient& ingredient) {

  //Unit toUnit = db.findBy<Unit>("name", ingredient.unit_name, "COLLATE NOCASE");
  //if (toUnit.exists()) {
  //  // convert? I don't remember the purpose of toUnit
  //qty.value = convert(s, unit, toUnit, ingredient.density);
  //}
  //ensure(toUnit.exists(), "ingredient must have a valid unit");

  IngredientQuantity qty;
  qty.recipe_id = recipe.id;
  qty.ingredient_id = ingredient.id;
  qty.value = quantity;
  qty.unit_id = unit.id;
  db.insert(qty);
}

// The instructions are all transformed to lower case
void parseRecipe(Database& db, Recipe& recipe) {

  auto h1 = Header1("PARSE RECIPE");

  db.deleteFrom<IngredientQuantity>("WHERE recipe_id = " + std::to_string(recipe.id));

  std::string str = recipe.instructions;
  transform(str.begin(), str.end(), str.begin(), ::tolower); 

  debug();

  while (!str.empty()) {

    std::string sentence = parseSentence(str);

    std::string instruction = parseWord(sentence);
    if (instruction == "ajouter") {

      double qty = parseFloat(sentence);
      Unit unit = parseDb<Unit>(db, sentence, "name");
      Ingredient ingredient = mustParseDb<Ingredient>(db, sentence, "name");

      ajouter(db, recipe, qty, unit, ingredient);

    } else {
      ensure(false, "L'instruction \"" + instruction + "\" n'est pas valide.");
    }
    trim(str);
  }

  //execute(recipe.instructions);
}

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

//};

#endif 
