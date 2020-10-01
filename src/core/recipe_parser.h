#ifndef _RECIPE_PARSER_H
#define _RECIPE_PARSER_H

#include "model.h"
#include "heda.h"

using namespace std;

//#define EXPRESSION_TYPE_NOT_FOUND -1
//#define EXPRESSION_TYPE_EMPTY 0
//#define EXPRESSION_TYPE_INT 1
//#define EXPRESSION_TYPE_FLOAT 2
//#define EXPRESSION_TYPE_UNIT 10
//#define EXPRESSION_TYPE_INGREDIENT 11
//#define EXPRESSION_TYPE_COMMAND 12
//
//// You can search the expressions by name, and/or by type
//// Use the database for the expressions?
//// The expression as the id of what it is referring to.
//class Expression {
//  public:
//    Expression(std::string val) : value(val) {}
//    std::string value;
//    int type;
//    int ref_id;
//};
//
//str
//parseCmd()
//ajouter()
//parseOneExpression(type: NUMBER)
//parseOneExpression(type: UNIT)
//parseOneExpression(type: INGREDIENT)
//
//
//
////Ajouter 3 cc levure_vivante
////Ajouter 3.5 t farine
////Ajouter 2 cs sucre
////Ajouter 2 cs huile
////Ajouter 1 cc sel
//
//class RecipeParser {
//  RecipeParser() {
//    // Load all the commands
//    // Load all the units to expressions
//    // Load all the ingredients to expressions
//  }
//  public:
//    std::set<Expression> expressions;
//};
//
//bool isNumber(std::string str) {
//  for (auto it = str.begin(); it != str.end(); it++) {
//    if (*it < '0' || *it > '9') {return false;}
//  }
//  return true;
//}
//
//// Parse the string character by charater.
//// If [ \n,], skip
//// If [0-9+-], than parse a number (Plus tard gérer un nombre débutant par un point: .5)
//// # is a comment and discard all the rest of the line
//// Otherwise, than parse an expression
//Expression parseOneExpression(std::string str) {
//
//  if (str.empty()) return Expression("");
//
//  if (str[0] == ' ' || str[0] == '\n' || str[0] == ',' || str[0] == '.') {
//    parseInstructions(str);
//  }
//    
//  if (str[0] >= '0' && str[0] <= '9') {
//      
//    string number = str.substr(0, str.find(' '));
//    if (isNumber(number)) {
//      int nb = atoi(number.c_str());
//      parseInstructions(str.substr(number.length()));
//    }
//  }
//
//  if (str[0] == 
//}

// Returns the substring until the next space. Updates the str parse what has been extracted.
std::string parseWord(std::string& str) {
 
  ltrim(str);
  string cmd = str.substr(0, str.find(' '));
  str = str.substr(cmd.length());
  return cmd;
}

double parseFloat(std::string& str) {
  
  string number = parseWord(str);

  char* pEnd;
  double val = strtod(number.c_str(), &pEnd);
  ensure(strlen(pEnd) == 0, "Was unable the parse a number with word = " + number);
  return val;
}

template<typename T>
T parseDb(Heda& heda, std::string& str, std::string columnName) {
  
  ltrim(str);

  int maxLength = std::max(str.length(), heda.db.getMaxLength<T>(columnName));

  for (int i = maxLength; i > 0; i--) {
    std::string sub = str.substr(0,i-1);
    T item = heda.db.findBy<T>(columnName, sub, "COLLATE NOCASE");
    if (item.exists()) {
      str = str.substr(sub.length());
      return item;
    }
  }

  ensure(false, "Was unable to parse a unit with sentence = " + str);
  T unreachable; return unreachable; // Damn complaining compiler
}

//select top 1 CR
//from table t
//order by len(CR) desc

void ajouter(Heda& heda, Recipe& recipe, double quantity, Unit& unit, Ingredient& ingredient) {

  Unit toUnit = heda.db.findBy<Unit>("name", ingredient.unit_name, "COLLATE NOCASE");
  ensure(toUnit.exists(), "ingredient must have a valid unit");

  IngredientQuantity qty;
  qty.recipe_id = recipe.id;
  qty.ingredient_id = ingredient.id;
  qty.value = quantity;
  //qty.value = convert(s, unit, toUnit, ingredient.density);
  qty.unit_id = unit.id;
  heda.db.insert(qty);
}

// The instructions are all transformed to lower case
void parseRecipe(Heda& heda, Recipe& recipe) {

  auto h1 = Header1("PARSE RECIPE");

  heda.db.deleteFrom<IngredientQuantity>("WHERE recipe_id = " + to_string(recipe.id));

  std::string str = recipe.instructions;
  transform(str.begin(), str.end(), str.begin(), ::tolower); 

  while (!str.empty()) {

    std::string instruction = parseWord(str);
    if (instruction == "ajouter") {

      double qty = parseFloat(str);
      Unit unit = parseDb<Unit>(heda, str, "name");
      Ingredient ingredient = parseDb<Ingredient>(heda, str, "name");

      ajouter(heda, recipe, qty, unit, ingredient);

    } else {
      ensure(false, "L'instruction \"" + instruction + "\" n'est pas valide.");
    }
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
