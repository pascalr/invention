#ifndef _HEDA_CONTROLLER_H
#define _HEDA_CONTROLLER_H

#include "heda.h"

class EnsureException : public exception {};
class UnitConversionException : public exception {};

class HedaController {
  public:

    void ensure(bool statement, const char* errorMessage) {
      if (!statement) {
        cerr << "\033[31mController error\033[0m: " << errorMessage << endl;
        throw EnsureException();
      }
    }
    
    bool is_processing_recipe = false;
    Recipe recipe_being_process;

    void process(Heda& heda, Recipe& recipe) {

      vector<int> ids;
      for (const IngredientQuantity& qty : heda.ingredient_quantities) {
        if (qty.recipe_id == recipe.id) {
          ids.push_back(qty.id);
        }
      }
      for (const int& id : ids) {
        heda.db.removeItem(heda.ingredient_quantities, id);
      }

      is_processing_recipe = true;
      recipe_being_process = recipe;
      execute(recipe.instructions);
      is_processing_recipe = false;
    }

    double convert(double val, Unit fromUnit, Unit toUnit, double density) {

      if (fromUnit.is_weight != toUnit.is_weight) {
        if (fromUnit.is_weight) {
          return val * (fromUnit.value / density) / toUnit.value;
        } else {
          return val * fromUnit.value / (toUnit.value / density);
        }
      }
      return val * fromUnit.value / toUnit.value;
    }

    HedaController(Heda& heda) : m_heda(heda) {

      // ------- FIXME -------- All those are commands to the HedaController, not to Heda itself (they would not belong in a recipee!)-------------
     
      // at throws a out of range exception 
      m_commands["stop"] = [&](ParseResult tokens) {heda.stop();};
      m_commands["pause"] = [&](ParseResult tokens) {heda.is_paused = true;};
      m_commands["unpause"] = [&](ParseResult tokens) {heda.is_paused = false;};
      m_commands["process"] = [&](ParseResult tokens) { // Calculate for a recipee
        // Delete all the ingredient quantities 
        Recipe recipe;
        string name = tokens.popNoun();
        ensure(heda.recipes.ifind(recipe, byName, name), "process command must have a valid recipe name");
        process(heda, recipe);
      };
      m_commands["genloc"] = [&](ParseResult tokens) {heda.generateLocations();};
      
      // -----------------------------------------------------------------------------------------------------
      
      m_commands["ajouter"] = [&](ParseResult tokens) {

        ensure(is_processing_recipe, "Must be processing recipe, because actually doing it is not implemented yet");

        double s = tokens.popScalaire();
        string unitName = tokens.popNoun();
        string ingredientName = tokens.popNoun();

        Unit unit;
        Unit toUnit;
        Ingredient ingredient;
        ensure(heda.units.ifind(unit, byName, unitName), "ajouter must have a valid unit name");
        ensure(heda.ingredients.ifind(ingredient, byName, ingredientName), "ajouter must have a valid ingredient name");
        ensure(heda.units.ifind(toUnit, byName, ingredient.unit_name), "ingredient must have a valid unit");

        IngredientQuantity qty;
        qty.recipe_id = recipe_being_process.id;
        qty.ingredient_id = ingredient.id;
        qty.value = s;
        //qty.value = convert(s, unit, toUnit, ingredient.density);
        qty.unit_id = unit.id;
        heda.db.insert(heda.ingredient_quantities, qty);
      };

      m_commands["home"] = [&](ParseResult tokens) {
        heda.pushCommand(make_shared<ReferencingCommand>(heda.axisR));
        heda.pushCommand(make_shared<ReferencingCommand>(heda.axisT));
        heda.pushCommand(make_shared<ReferencingCommand>(heda.axisH));
        heda.pushCommand(make_shared<MoveCommand>(heda.axisT, CHANGE_LEVEL_ANGLE_HIGH));
        heda.pushCommand(make_shared<ReferencingCommand>(heda.axisV));
        heda.pushCommand(make_shared<GotoCommand>(PolarCoord(heda.config.home_position_x, heda.config.home_position_y, heda.config.home_position_t)));
        heda.pushCommand(make_shared<OpenGripCommand>());
      };

      m_commands["gohome"] = [&](ParseResult tokens) {
        heda.pushCommand(make_shared<GotoCommand>(PolarCoord(heda.config.home_position_x, heda.config.home_position_y, heda.config.home_position_t)));
      };

      m_commands["store"] = [&](ParseResult tokens) {
        string name = "";
        try {name = tokens.popNoun();} catch (const MissingArgumentException& e) {/*It's ok it's optional.*/}
        heda.pushCommand(make_shared<StoreCommand>(name));
      };
      m_commands["grip"] = [&](ParseResult tokens) {
        unsigned long id = tokens.popPositiveInteger();
        Jar jar;
        if (heda.jars.get(jar, id)) {
          heda.pushCommand(make_shared<GripCommand>(jar));
        } // TODO Handle error
      };
      m_commands["pickup"] = [&](ParseResult tokens) {
        // TODO: popLocation? worth it? wait to see...
        int id = tokens.popPositiveInteger();
        string locationName = tokens.popNoun();
        Location loc;
        if (heda.locations.byName(loc, locationName) < 0) {
          cout << "A location name was given, but it was not found. Aborting pickup.";
          return;
        }
        Jar jar;
        if (heda.jars.get(jar, id)) {
          heda.pushCommand(make_shared<PickupCommand>(jar, loc));
          return;
        } // TODO Handle error
        cout << "Oups. No jar were found with this id." << endl;
      };
      m_commands["move"] = [&](ParseResult tokens) {
        char axis = tokens.popAxis();
        double dest = tokens.popScalaire();
        heda.move(Movement(axis,dest));
      };
      m_commands["sweep"] = [&](ParseResult tokens) {
        heda.pushCommand(make_shared<SweepCommand>());
      };
      m_commands["grab"] = [&](ParseResult tokens) {
        double strength = tokens.popScalaire();
        cout << "Executing grab with strength = " << strength << endl;
        heda.pushCommand(make_shared<GrabCommand>(strength));
      };
      m_commands["capture"] = [&](ParseResult tokens) {heda.capture();};
      m_commands["detect"] = [&](ParseResult tokens) {heda.pushCommand(make_shared<DetectCommand>());};
      m_commands["parse"] = [&](ParseResult tokens) {heda.pushCommand(make_shared<ParseCodesCommand>());};
      m_commands["pinpoint"] = [&](ParseResult tokens) {heda.pinpoint();};
      //m_commands["calibrate"] = [&](ParseResult tokens) {heda.calibrate();};
      m_commands["putdown"] = [&](ParseResult tokens) {heda.pushCommand(make_shared<PutdownCommand>());};
      m_commands["fetch"] = [&](ParseResult tokens) {// Fetch an ingredient
        string ingredientName = tokens.popNoun();
        //heda.fetch(ingredientName);
      }; 
      m_commands["photo"] = [&](ParseResult tokens) {
        Mat mat;
        heda.captureFrame(mat);
      };
      m_commands["goto"] = [&](ParseResult tokens) {
        double h = tokens.popScalaire();
        double v = tokens.popScalaire();
        double t = tokens.popScalaire();
        heda.pushCommand(make_shared<GotoCommand>(PolarCoord(h,v,t)));
      };
      m_commands["open"] = [&](ParseResult tokens) {
        heda.pushCommand(make_shared<OpenGripCommand>());
      };
    
    }

    std::function<void(ParseResult)> getCommand(string name) {
       
      for (auto item : m_commands) { // I am not using map[key] because I want to compare without case. OPTIMIZE better data structure to do this.
        if (iequals(item.first, name)) {
          return item.second;
        }
      }
      return 0;
    }

    // Commands can be split with a newline (\n)
    // FIXME: Don't execute the commands as soon as I get them. Stack them. Add a ServerCommand or something to the stack.
    // But check for the stop command.
    void execute(string str) {

      if (str.empty()) {return;}

      string::size_type pos = str.find('\n');
      string cmd = (pos == string::npos) ? str : str.substr(0, pos);

      cerr << "Executing cmd = " << cmd << "\n";

      string cmdName;
      Parser parser;
      ParseResult result;
      parser.parse(result, cmd);

      auto func = getCommand(result.getCommand());
      if (func) {
        try {
          func(result);
        } catch (const EnsureException& e) {
        } catch (const MissingArgumentException& e) {cerr << "Caught a missing argument exception" << endl;}
      } else {cerr << "Error unkown command: " << result.getCommand() << "\n"; return;}

      if (pos != string::npos) {execute(str.substr(pos+1));}
    }

    void loopCommandStack(Reader& reader) {
      while (true) {
        if(reader.inputAvailable()) {
          string cmd = getInputLine(reader);
          execute(cmd);
        }
        this_thread::sleep_for(chrono::milliseconds(m_heda.handleCommandStack()));
      }
    }

    std::unordered_map<std::string, std::function<void(ParseResult)>> m_commands;

    Heda& m_heda;
};

#endif
