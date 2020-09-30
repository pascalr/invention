#ifndef _HEDA_CONTROLLER_H
#define _HEDA_CONTROLLER_H

#include "heda.h"

class UnitConversionException : public exception {};

// Heda controller stack command
// But we don't care for that right now. I do only one command at a time.
// This will be important when multiple users.
//class StackCommand {
//  public:
//    std::string name;
//    std::function<void(ParseResult)> func;
//};
//std::vector<std::function<void(ParseResult)>> HedaController::m_stack;

class HedaController {
  public:

    bool is_processing_recipe = false;
    Recipe recipe_being_process;

    void process(Heda& heda, Recipe& recipe) {

      heda.db.deleteFrom<IngredientQuantity>("WHERE recipe_id = " + to_string(recipe.id));

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
      m_commands["dismiss"] = [&](ParseResult tokens) {heda.waiting_message = ""; heda.fatal_message = "";};
      m_commands["pause"] = [&](ParseResult tokens) {heda.is_paused = true;};
      m_commands["done"] = [&](ParseResult tokens) {heda.user_response = "done";};
      m_commands["unpause"] = [&](ParseResult tokens) {heda.is_paused = false;};
      m_commands["test"] = [&](ParseResult tokens) {heda.pushCommand(make_shared<TestCommand>());};
      m_commands["process"] = [&](ParseResult tokens) { // Calculate for a recipee
        // Delete all the ingredient quantities 
        string name = tokens.popNoun();
        Recipe recipe = heda.db.findBy<Recipe>("name", name, "COLLATE NOCASE");
        ensure(recipe.exists(), "process command must have a valid recipe name");
        process(heda, recipe);
      };
      //m_commands["genloc"] = [&](ParseResult tokens) {heda.generateLocations();};
      m_commands["calibrate"] = [&](ParseResult tokens) {
        string jarFormatName = tokens.popNoun();

        JarFormat format = heda.db.findBy<JarFormat>("name", jarFormatName, "COLLATE NOCASE");
        ensure(format.exists(), "Calibrate needs a valid jar format.");
        heda.calibrate(format);
      };
      //m_commands["closeup"] = [&](ParseResult tokens) { // Move closer to the detected codes to get a better picture.
      //  debug();
      //  DetectedHRCode code;
      //  unsigned long id = tokens.popPositiveInteger();
      //  ensure(heda.codes.get(code, id), "closeup must have a valid code id");
      //  Jar jar;
      //  heda.pushCommand(make_shared<CloseupCommand>(code, jar));
      //};
      m_commands["storeall"] = [&](ParseResult tokens) { // store all detected jar, starting with the tallest

        vector<DetectedHRCode> codes = heda.db.all<DetectedHRCode>("ORDER BY lid_y DESC");
        //heda.codes.order(byLidY, false);
        for (DetectedHRCode& code : codes) {
          heda.pushCommand(make_shared<StoreDetectedCommand>(code));
        }
        heda.pushCommand(make_shared<GotoCommand>(PolarCoord(heda.unitH(heda.config.home_position_x, 0, 0), heda.unitV(heda.config.home_position_y), heda.config.home_position_t)));
      };
      m_commands["nodup"] = [&](ParseResult tokens) {
        removeNearDuplicates(heda);
      };
      //m_commands["moveover"] = [&](ParseResult tokens) {
      //  moveOver(Heda& heda);
      //};
      
      // -----------------------------------------------------------------------------------------------------
      
      // hover command => Move on top of the thing in x and z, at the moving height of the shelf.
      m_commands["hover"] = [&](ParseResult tokens) {
        double x = tokens.popScalaire();
        double z = tokens.popScalaire();
        heda.pushCommand(make_shared<HoverCommand>(x,z,heda.config.gripper_radius));
      };
      
      
      m_commands["ajouter"] = [&](ParseResult tokens) {

        ensure(is_processing_recipe, "Must be processing recipe, because actually doing it is not implemented yet");

        double s = tokens.popScalaire();
        string unitName = tokens.popNoun();
        string ingredientName = tokens.popNoun();

        Unit unit = heda.db.findBy<Unit>("name", unitName, "COLLATE NOCASE");
        ensure(unit.exists(), "ajouter must have a valid unit name");

        Ingredient ingredient = heda.db.findBy<Ingredient>("name", ingredientName, "COLLATE NOCASE");
        ensure(ingredient.exists(), "ajouter must have a valid ingredient name");

        Unit toUnit = heda.db.findBy<Unit>("name", ingredient.unit_name, "COLLATE NOCASE");
        ensure(toUnit.exists(), "ingredient must have a valid unit");

        IngredientQuantity qty;
        qty.recipe_id = recipe_being_process.id;
        qty.ingredient_id = ingredient.id;
        qty.value = s;
        //qty.value = convert(s, unit, toUnit, ingredient.density);
        qty.unit_id = unit.id;
        heda.db.insert(qty);
      };
      
      m_commands["ref"] = [&](ParseResult tokens) {
        char axisName = tokens.popAxis();
        Axis* axis = heda.axisByName(axisName);
        ensure(axis != 0, "ref command expects a valid axis name");
        heda.pushCommand(make_shared<ReferencingCommand>(*axis));
      };

      m_commands["home"] = [&](ParseResult tokens) {
        heda.pushCommand(make_shared<ReferencingCommand>(heda.axisR));
        heda.pushCommand(make_shared<ReferencingCommand>(heda.axisT));
        heda.pushCommand(make_shared<ReferencingCommand>(heda.axisH));
        heda.pushCommand(make_shared<MoveCommand>(heda.axisT, CHANGE_LEVEL_ANGLE_HIGH));
        heda.pushCommand(make_shared<ReferencingCommand>(heda.axisV));
        heda.pushCommand(make_shared<GotoCommand>(PolarCoord(heda.unitH(heda.config.home_position_x, 0, 0), heda.unitV(heda.config.home_position_y), heda.config.home_position_t)));
        heda.pushCommand(make_shared<OpenGripCommand>());
      };

      m_commands["gohome"] = [&](ParseResult tokens) {
        heda.pushCommand(make_shared<GotoCommand>(PolarCoord(heda.unitH(heda.config.home_position_x, 0, 0), heda.unitV(heda.config.home_position_y), heda.config.home_position_t)));
      };

      m_commands["store"] = [&](ParseResult tokens) {
        heda.pushCommand(make_shared<SweepCommand>());

        vector<DetectedHRCode> codes = heda.db.all<DetectedHRCode>("ORDER BY lid_y DESC");
        for (DetectedHRCode& code : codes) {
          heda.pushCommand(make_shared<StoreDetectedCommand>(code));
        }
        heda.pushCommand(make_shared<GotoCommand>(PolarCoord(heda.unitH(heda.config.home_position_x, 0, 0), heda.unitV(heda.config.home_position_y), heda.config.home_position_t)));
      };
      //m_commands["store"] = [&](ParseResult tokens) {
      //  string name = "";
      //  try {name = tokens.popNoun();} catch (const MissingArgumentException& e) {/*It's ok it's optional.*/}
      //  heda.pushCommand(make_shared<StoreCommand>(name));
      //};
      //m_commands["grip"] = [&](ParseResult tokens) {
      //  unsigned long id = tokens.popPositiveInteger();
      //  Jar jar;
      //  if (heda.jars.get(jar, id)) {
      //    heda.pushCommand(make_shared<GripCommand>(jar));
      //  } // TODO Handle error
      //};
      //m_commands["pickup"] = [&](ParseResult tokens) {
      //  // TODO: popLocation? worth it? wait to see...
      //  int id = tokens.popPositiveInteger();
      //  string locationName = tokens.popNoun();
      //  Location loc;
      //  if (heda.locations.byName(loc, locationName) < 0) {
      //    cout << "A location name was given, but it was not found. Aborting pickup.";
      //    return;
      //  }
      //  Jar jar;
      //  if (heda.jars.get(jar, id)) {
      //    heda.pushCommand(make_shared<PickupCommand>(jar, loc));
      //    return;
      //  } // TODO Handle error
      //  cout << "Oups. No jar were found with this id." << endl;
      //};
      m_commands["move"] = [&](ParseResult tokens) {
        char axisName = tokens.popAxis();
        double dest = tokens.popScalaire();
       
        Axis* axis = heda.axisByName(axisName);
        ensure(axis != 0, "ref command expects a valid axis name");
        heda.pushCommand(make_shared<MoveCommand>(*axis, dest));
      };
      m_commands["sweep"] = [&](ParseResult tokens) {
        heda.pushCommand(make_shared<SweepCommand>());
      };
      m_commands["loadcfg"] = [&](ParseResult tokens) {
        heda.loadConfig();
      };
      m_commands["grab"] = [&](ParseResult tokens) {
        double strength = tokens.popScalaire();
        cout << "Executing grab with strength = " << strength << endl;
        heda.pushCommand(make_shared<GrabCommand>(strength));
      };
      m_commands["capture"] = [&](ParseResult tokens) {heda.capture();};
      m_commands["detect"] = [&](ParseResult tokens) {heda.pushCommand(make_shared<DetectCommand>());};
      m_commands["parse"] = [&](ParseResult tokens) {heda.pushCommand(make_shared<ParseCodesCommand>());};
      m_commands["pinpoint"] = [&](ParseResult tokens) {heda.pushCommand(make_shared<PinpointCommand>());};
      m_commands["putdown"] = [&](ParseResult tokens) {heda.pushCommand(make_shared<PutdownCommand>());};
      m_commands["fetch"] = [&](ParseResult tokens) {// Fetch an ingredient

        string ingredientName = tokens.popNoun();

        Ingredient ingredient = heda.db.findBy<Ingredient>("name", ingredientName, "COLLATE NOCASE");
        ensure(ingredient.exists(), "Could not find an ingredient with the name " + ingredientName);

        Jar jar = heda.db.findBy<Jar>("ingredient_id", ingredient.id);
        ensure(ingredient.exists(), "Could not find a jar that contains the ingredient " + ingredientName);

        heda.pushCommand(make_shared<FetchCommand>(jar));
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
      m_commands["getto"] = [&](ParseResult tokens) {
        double x = tokens.popScalaire();
        double y = tokens.popScalaire();
        double z = tokens.popScalaire();
        heda.pushCommand(make_shared<GotoCommand>(heda.toPolarCoord(UserCoord(x,y,z), heda.config.gripper_radius)));
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
          // Return a bad request to the server when an exception like this occur. This is different thant Heda. This is HedaController.
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
        try {
          this_thread::sleep_for(chrono::milliseconds(m_heda.handleCommandStack()));
        } catch (const EnsureException& e) {
          m_heda.fatal_message = e.message;
          m_heda.stop();
        }
      }
    }

    std::unordered_map<std::string, std::function<void(ParseResult)>> m_commands;

    Heda& m_heda;
};

#endif
