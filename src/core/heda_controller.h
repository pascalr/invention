#ifndef _HEDA_CONTROLLER_H
#define _HEDA_CONTROLLER_H

#include "heda.h"

class HedaController {
  public:

    HedaController(Heda& heda) : m_heda(heda) {
     
      // at throws a out of range exception 
      m_commands["stop"] = [&](ParseResult tokens) {heda.stop();};
      m_commands["home"] = [&](ParseResult tokens) {heda.home();};
      m_commands["gohome"] = [&](ParseResult tokens) {heda.gohome();};
      m_commands["store"] = [&](ParseResult tokens) {
        string name = "";
        try {name = tokens.popNoun();} catch (const MissingArgumentException& e) {/*It's ok it's optional.*/}
        heda.store(name);
      };
      m_commands["grip"] = [&](ParseResult tokens) {
        unsigned long id = tokens.popPositiveInteger();
        heda.grip(id);
      };
      m_commands["pickup"] = [&](ParseResult tokens) {
        int id = tokens.popPositiveInteger();
        string locationName = tokens.popNoun();
        for (const Jar& jar : heda.jars.items) {
          if (jar.id == id) {
            heda.pickup(jar, locationName);
            return;
          }
        }
        cout << "Oups. No jar were found with this id." << endl;
      };
      m_commands["move"] = [&](ParseResult tokens) {
        char axis = tokens.popAxis();
        double dest = tokens.popScalaire();
        heda.move(Movement(axis,dest));
      };
      m_commands["sweep"] = [&](ParseResult tokens) {heda.sweep();};
      m_commands["grab"] = [&](ParseResult tokens) {
        double strength = tokens.popScalaire();
        cout << "Executing grab with strength = " << strength << endl;
        heda.grab(strength);
      };
      m_commands["capture"] = [&](ParseResult tokens) {heda.capture();};
      m_commands["detect"] = [&](ParseResult tokens) {heda.detect();};
      m_commands["parse"] = [&](ParseResult tokens) {heda.parse();};
      m_commands["genloc"] = [&](ParseResult tokens) {heda.generateLocations();}; // FIXME: The user should not be able to do this easily..
      m_commands["pinpoint"] = [&](ParseResult tokens) {heda.pinpoint();};
      m_commands["calibrate"] = [&](ParseResult tokens) {heda.calibrate();};
      m_commands["putdown"] = [&](ParseResult tokens) {heda.putdown();};
      m_commands["fetch"] = [&](ParseResult tokens) {// Fetch an ingredient
        string ingredientName = tokens.popNoun();
        heda.fetch(ingredientName);
      }; 
      m_commands["photo"] = [&](ParseResult tokens) {
        Mat mat;
        heda.captureFrame(mat);
      };
      m_commands["goto"] = [&](ParseResult tokens) {
        double x = tokens.popScalaire();
        double y = tokens.popScalaire();
        double t = tokens.popScalaire();
        heda.moveTo(PolarCoord(x, y, t));
      };
      m_commands["open"] = [&](ParseResult tokens) {
        heda.openJaw();
      };
    
    }

    // Commands can be split with a semicolon (;)
    // FIXME: Don't execute the commands as soon as I get them. Stack them. Add a ServerCommand or something to the stack.
    // But check for the stop command.
    void execute(string str) {

      string::size_type pos = str.find(';');
      string cmd = (pos == string::npos) ? str : str.substr(0, pos);

      cerr << "Executing cmd = " << cmd << "\n";

      string cmdName;
      Parser parser;
      ParseResult result;
      parser.parse(result, cmd);

      bool found = false;
      for (auto item : m_commands) {
        if (iequals(item.first, result.getCommand())) {
          item.second(result);
          found = true; break;
        }
      }
      if (!found) {
        cerr << "Error unkown command: " << result.getCommand();
      }

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
