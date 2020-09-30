#ifndef _HEDA_CONTROLLER_H
#define _HEDA_CONTROLLER_H

#include "pinpoint.h"
#include "heda.h"
#include <thread>
#include <chrono>
#include <iostream>

using namespace std;

class UnitConversionException : public exception {};

//namespace Heda {

void writeSlave(Heda& heda, std::string cmd) {
  heda.m_writer << cmd;
  
  // Loops until the message MESSAGE_DONE has been received.
  while (true) {

    heda.ensureActive();

    if (heda.m_reader.inputAvailable()) {

      string str = getInputLine(heda.m_reader);
      if (str == MESSAGE_DONE) break;
    }
      
    this_thread::sleep_for(chrono::milliseconds(10));
  }
}

void openGrip(Heda& heda) {

  auto h5 = Header5("OPEN GRIP"); 

  writeSlave(heda, "r");

  heda.is_gripping = false;
}

void reference(Heda& heda, Axis& axis) {

  std::string cmd = "h" + string(1, axis.id);
  auto h5 = Header5("HOME("+cmd+")"); 

  writeSlave(heda, cmd);

  if (axis.id == AXIS_H) {
    heda.m_position.h = REFERENCE_POSITION_H;
  } else if (axis.id == AXIS_V) {
    heda.m_position.v = REFERENCE_POSITION_V;
  } else if (axis.id == AXIS_T) {
    heda.m_position.t = REFERENCE_POSITION_T;
  //} else if (axis.id == AXIS_R) {
  }
  axis.is_referenced = true;
}

void move(Heda& heda, Axis& axis, double destination) {

  std::string cmd = "m" + string(1, axis.id) + to_string(destination);
  auto h5 = Header5("MOVE("+cmd+")"); 

  writeSlave(heda, cmd);

  if (axis.id == AXIS_H) {
    heda.m_position.h = destination;
  } else if (axis.id == AXIS_V) {
    heda.m_position.v = destination;
  } else if (axis.id == AXIS_T) {
    heda.m_position.t = destination;
  //} else if (axis.id == AXIS_R) {
  }
}

void validateGoto(Heda& heda, PolarCoord c) {
  ensure(c.h >= heda.config.minH(), "The goto destination h must be higher than the minimum.");
  ensure(c.v >= heda.config.minV(), "The goto destination v must be higher than the minimum.");
  ensure(c.t >= heda.config.minT(), "The goto destination t must be higher than the minimum.");
}

void gotoPolar(Heda& heda, PolarCoord destination) {

  std::string cmd = to_string(destination.h) + " " + to_string(destination.v) + " " + to_string(destination.t);
  auto h4 = Header4("GOTO("+cmd+")");

  validateGoto(heda, destination);

  PolarCoord position = heda.getPosition();

  Shelf currentShelf; heda.shelfByHeight(currentShelf, heda.unitY(position.v));
  Shelf destinationShelf; heda.shelfByHeight(destinationShelf, heda.unitY(destination.v));
    
  double positionT = position.t;

  // must change level
  if (currentShelf.id != destinationShelf.id) {

    move(heda, heda.axisV, heda.unitV(currentShelf.moving_height));

    positionT = (position.h < heda.config.middleH()) ? CHANGE_LEVEL_ANGLE_HIGH : CHANGE_LEVEL_ANGLE_LOW;
    move(heda, heda.axisT, positionT);
  }

  move(heda, heda.axisV, destination.v);

  // If moving theta would colide, move x first
  double deltaH = cosd(destination.t) * heda.config.gripper_radius;
  double hIfTurnsFirst = position.h - deltaH;

  cout << "position.h: " << position.h << endl;
  cout << "deltaH: " << deltaH << endl;
  cout << "hIfTurnsFirst: " << hIfTurnsFirst << endl;
  cout << "destination.h: " << destination.h << endl;
  cout << "positionT: " << positionT << endl;

  if (hIfTurnsFirst < 0.0 || hIfTurnsFirst > heda.config.max_h) {

    // To avoid collision, if the arm goes over the middle, go to the middle before moving
    if ((positionT < 90.0 && destination.t > 90.0) || (positionT > 90.0 && destination.t < 90.0)) {
      move(heda, heda.axisT, 90.0);
    }

    move(heda, heda.axisH, destination.h);
    move(heda, heda.axisT, destination.t);
  } else {
    move(heda, heda.axisT, destination.t);
    move(heda, heda.axisH, destination.h);
  }
}

void detectCodes(Heda& heda, vector<DetectedHRCode>& detected, Mat& frame, PolarCoord c) {

  cout << "Running detect code." << endl;
  HRCodeParser parser(0.2, 0.2);
  vector<HRCode> positions;
  parser.findHRCodes(frame, positions, 100);

  if (!positions.empty()) {
    for (auto it = positions.begin(); it != positions.end(); ++it) {
      cout << "Detected one HRCode!!!" << endl;
      DetectedHRCode d(*it, c);
      detected.push_back(d);
    }
    return;
  }
  cout << "No codes were detected..." << endl;
}

void detect(Heda& heda) {
  
  auto h5 = Header5("DETECT");

  Mat frame;
  heda.captureFrame(frame);
  vector<DetectedHRCode> detected;
  detectCodes(heda, detected, frame, heda.getPosition());
  for (DetectedHRCode& it : detected) {
    heda.db.insert(it);
  }
}

void pinpoint(Heda& heda) {
  
  auto h2 = Header2("PINPOINT");

  for (DetectedHRCode& code : heda.db.all<DetectedHRCode>()) {
    pinpointCode(heda, code);
    heda.db.update(code);
  }
}

void parse(Heda& heda) {
  
  auto h2 = Header2("PARSE");

  for (DetectedHRCode& code : heda.db.all<DetectedHRCode>()) {
    parseCode(heda, code);
    heda.db.update(code);
  }
}

class Cluster {
  public:
    int idxKept;
    vector<int> indicesRemoved;
};

double distanceSquare(const UserCoord& c1, const UserCoord& c2) {
  return (c1.x-c2.x)*(c1.x-c2.x) + (c1.y-c2.y)*(c1.y-c2.y) + (c1.z-c2.z)*(c1.z-c2.z); 
}

double detectedDistanceSquared(const DetectedHRCode& c1, const DetectedHRCode& c2) {
  Vector2f lid1; lid1 << c1.lid_coord.x, c1.lid_coord.z;
  Vector2f lid2; lid2 << c2.lid_coord.x, c2.lid_coord.z;
  return (lid1 - lid2).squaredNorm();
}

// Inefficient algorithm when n is large, but in my case n is small. O(n^2) I believe.
void removeNearDuplicates(Heda& heda) {

  double epsilon = pow(HRCODE_OUTER_DIA * 1.5, 2);
  //removeNearDuplicates(heda.codes, detectedDistanceSquared, epsilon);
  vector<DetectedHRCode> codes = heda.db.all<DetectedHRCode>();
  vector<int> ids;
  vector<Cluster> clusters;
  for (unsigned int i = 0; i < codes.size(); i++) {

    DetectedHRCode& code = codes[i];
    // An item can only belong to one cluster. So discard if already belongs to one.
    if (count(ids.begin(), ids.end(), code.id) > 0) continue;

    Cluster cluster;
    cluster.idxKept = i;
    for (unsigned int j = i+1; j < codes.size(); j++) {
      DetectedHRCode& other = codes[j];
      if (detectedDistanceSquared(code, other) < epsilon) {
        ids.push_back(other.id);
        cluster.indicesRemoved.push_back(j);
      }
    }
    clusters.push_back(cluster);
  }
  for (Cluster& cluster : clusters) {

    double sumX = codes[cluster.idxKept].lid_coord.x;
    double sumZ = codes[cluster.idxKept].lid_coord.z;
    for (int &idx : cluster.indicesRemoved) {
      sumX += codes[idx].lid_coord.x;
      sumZ += codes[idx].lid_coord.z;
    }
    DetectedHRCode& code = codes[cluster.idxKept];
    code.lid_coord.x = sumX / (cluster.indicesRemoved.size() + 1);
    code.lid_coord.z = sumZ / (cluster.indicesRemoved.size() + 1);
    heda.db.update(code);
  }

  for (int &id : ids) {
    heda.db.deleteFrom<DetectedHRCode>("WHERE id = " + to_string(id));
  }
}

void sweep(Heda& heda) {

  auto h2 = Header2("SWEEP");

  heda.db.clear<DetectedHRCode>();
  
  PolarCoord max(heda.config.max_h, heda.config.max_v, 90.0);

  int nStepX = 10;
  int nStepZ = 4;

  double xMin = heda.unitX(heda.config.minH(), 0.0, 0.0);
  double xMax = heda.unitX(heda.config.max_h, 0.0, 0.0);
  double xDiff = xMax - xMin;

  double zMin = heda.unitZ(90.0, heda.config.gripper_radius);
  double zMax = heda.working_shelf.depth;
  double zDiff = zMax - zMin;
 
  bool xUp = true;
  for (int j = 0; j <= nStepZ; j++) {
    for (int i = 0; i <= nStepX; i++) {

      double x = (i*1.0*xDiff/nStepX)+xMin;
      double z = (j*1.0*zDiff/nStepZ)+zMin;

      if (!xUp) {x = xMax + xMin - x;}

      UserCoord c(x, heda.config.detect_height, z);
      gotoPolar(heda, heda.toPolarCoord(c,heda.config.gripper_radius));
      detect(heda);
    }
    xUp = !xUp;
  }

  pinpoint(heda);
  parse(heda);
  gotoPolar(heda, PolarCoord(heda.unitH(heda.config.home_position_x, 0, 0), heda.unitV(heda.config.home_position_y), heda.config.home_position_t));
  removeNearDuplicates(heda);
}


//// Heda controller stack command
//class StackCommand {
//  public:
//    std::string name;
//    std::function<void(ParseResult)> func;
//    bool is_done;
//};

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

    HedaController(Heda& heda) : heda(heda) {

      // ------- FIXME -------- All those are commands to the HedaController, not to Heda itself (they would not belong in a recipee!)-------------
   
      m_commands["move"] = [&](ParseResult tokens) {
        char axisName = tokens.popAxis();
        double dest = tokens.popScalaire();
       
        Axis* axis = heda.axisByName(axisName);
        ensure(axis != 0, "ref command expects a valid axis name");
        move(heda, *axis, dest);
      };
      
      m_commands["home"] = [&](ParseResult tokens) {
        reference(heda, heda.axisR);
        reference(heda, heda.axisT);
        reference(heda, heda.axisH);
        move(heda, heda.axisT, CHANGE_LEVEL_ANGLE_HIGH);
        reference(heda, heda.axisV);
        gotoPolar(heda, PolarCoord(heda.unitH(heda.config.home_position_x, 0, 0), heda.unitV(heda.config.home_position_y), heda.config.home_position_t));
        openGrip(heda);
      };
      
      m_commands["sweep"] = [&](ParseResult tokens) {sweep(heda);};

      // ----------------------------------------------------------

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
        } catch (const StoppedException& e) {
          // Nothing to do
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
          this_thread::sleep_for(chrono::milliseconds(heda.handleCommandStack()));
        } catch (const EnsureException& e) {
          heda.fatal_message = e.message;
          heda.stop();
        }
      }
    }

    std::unordered_map<std::string, std::function<void(ParseResult)>> m_commands;
    
    //std::unordered_map<std::string, std::function<void(ParseResult)>> commands;
    //std::vector<std::function<void(ParseResult)>> stack;

    Heda& heda;
};

#endif
