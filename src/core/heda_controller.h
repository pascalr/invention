#ifndef _HEDA_CONTROLLER_H
#define _HEDA_CONTROLLER_H

#include "heda.h"

class HedaController {
  public:

    HedaController(Heda& heda) : m_heda(heda) {
     
      // at throws a out of range exception 
      m_commands["stop"] = [&](ParseResult tokens) {heda.stop();};
      m_commands["pause"] = [&](ParseResult tokens) {heda.is_paused = true;};
      m_commands["unpause"] = [&](ParseResult tokens) {heda.is_paused = false;};

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
      m_commands["genloc"] = [&](ParseResult tokens) {heda.generateLocations();}; // FIXME: The user should not be able to do this easily..
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

      string::size_type pos = str.find('\n');
      string cmd = (pos == string::npos) ? str : str.substr(0, pos);

      cerr << "Executing cmd = " << cmd << "\n";

      string cmdName;
      Parser parser;
      ParseResult result;
      parser.parse(result, cmd);

      auto func = getCommand(result.getCommand());
      if (func) {func(result);}
      else {cerr << "Error unkown command: " << result.getCommand() << "\n"; return;}

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
