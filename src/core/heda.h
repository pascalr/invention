#ifndef _HEDA_H
#define _HEDA_H

class Heda;

#include "position.h"
#include "../lib/serial.h"
#include "../lib/opencv.h"
#include "reader/reader.h"
#include "writer/writer.h"
#include "writer/log_writer.h"

#include "command_stack.h"

#include "parser.h"

#include "jar_parser.h"

#include "database.h"

class HedaException : public exception {};

class InitVideoException : public HedaException {};
class InitArduinoException : public HedaException {};
class MissingHRCodeException : public HedaException {};
class TooManyHRCodeException : public HedaException {};
class FrameCaptureException : public HedaException {};

class MissingConfigException : public exception {};
class NoWorkingShelfException : public exception {};

#include <mutex>

enum HedaCommandType {
  STOP,
  HOME,
  GOHOME,
  STORE,
  GRAB,
  PICKUP,
  MOVE,
  GENLOC,
  SWEEP,
  GRIP,
  PINPOINT,
  DETECT,
  PUTDOWN,
  FETCH,
  GOTO,
  REFERENCE,
  OPEN
};


// TODO: Rename x and y to h and v at some point.
#define AXIS_H 'x'
#define AXIS_V 'y'
#define AXIS_T 't'
#define AXIS_R 'r'

class Axis {
  public:
    Axis(char id) : id(id) {}
    char id;
    bool is_referenced = false;
};


class AvailableHedaCommand {
  public:
    AvailableHedaCommand() {}
    AvailableHedaCommand(string name, std::function<void(ParseResult)> func, string help) : name(name), func(func), help(help) {}
    string name;
    std::function<void(ParseResult)> func;
    string help;
};


class HedaCommand {
  public:

    virtual string str() = 0;

    virtual void start(Heda& heda) = 0;

    virtual bool isDone(Heda& heda) {
      return true;
    }

    virtual void doneCallback(Heda& heda) {}
};

class SlaveCommand : public HedaCommand {
  public:
    
    SlaveCommand(string cmd) : cmd(cmd) {}

    virtual string str() {
      return "SlaveCommand(" + cmd + ")";
    }

    void start(Heda& heda);

    bool isDone(Heda& heda);

    string cmd;
};

class ReferencingCommand : public SlaveCommand {
  public:
    
    ReferencingCommand(Axis& axis0) : SlaveCommand("h" + string(1, axis0.id)), axis(axis0) {}
    
    string str() {
      return "home " + string(1, axis.id);
    }

    void doneCallback(Heda& heda);

    Axis& axis;
};

class MoveCommand : public SlaveCommand {
  public:
    
    MoveCommand(Axis& axis0, double destination0) : SlaveCommand("m" + string(1, axis0.id) + to_string(destination0)), axis(axis0), destination(destination0) {
    }
    
    string str() {
      return "move " + string(1, axis.id) + to_string(destination);
    }

    void doneCallback(Heda& heda);

    Axis& axis;
    double destination;
};

class GotoCommand : public HedaCommand {
  public:
    GotoCommand(PolarCoord destination) : destination(destination) {}
    
    string str() {
      return "goto " + to_string(destination.h) + " " + to_string(destination.v) + " " + to_string(destination.t);
    }

    void start(Heda& heda);

    bool isDone(Heda& heda);

    PolarCoord destination;
    vector<MoveCommand> mvts;
    vector<MoveCommand>::iterator currentMove;
};

// Heda is the source of truth. It is the only class that should read the arduino serial and write to it.
// It has a stack of commands to execute.
// It keeps a copy of the serial it has read if it required at some point.

class Heda {
  public:

    Heda(Writer& writer, Reader& reader, Database &db, Writer& serverWriter) : axisH(AXIS_H), axisV(AXIS_V), axisT(AXIS_T), axisR(AXIS_R),
              m_reader(reader),
              m_writer(writer),
              server_writer(serverWriter),
              stack_writer("\033[38mStack\033[0m"),
              db(db) {
    
      setupCommands();

      loadDb();
    }

    void loadDb() {
      db.load(configs);
      if (configs.empty()) {throw MissingConfigException();}
      config = *configs.begin();
      db.load(shelves);
      db.load(codes);
      db.load(jar_formats);
      db.load(jars);
      db.load(locations);
      db.load(ingredients);
    }

    void addAvailableCommand(HedaCommandType type, const char* name, std::function<void(ParseResult)> func, const char* help="") {
      m_commands[type] = AvailableHedaCommand(name, func, help);
    }

    AvailableHedaCommand getAvailableCommand(HedaCommandType type) {
      return m_commands[type];
    }

    void setupCommands() {
     
      // at throws a out of range exception 
      addAvailableCommand(STOP, "stop", [&](ParseResult tokens) {stop();}, "Stop everything the slave is doing.");
      addAvailableCommand(HOME, "home", [&](ParseResult tokens) {home();}, "References all axes then moves to the home position.");
      addAvailableCommand(GOHOME, "gohome", [&](ParseResult tokens) {gohome();}, "Moves to the home position.");
      addAvailableCommand(STORE, "store", [&](ParseResult tokens) {
        string name = "";
        try {name = tokens.popNoun();} catch (const MissingArgumentException& e) {/*It's ok it's optional.*/}
        store(name);
      }, "Store the gripped jar to the location given. If no location is specified, store in the next available location.");
      addAvailableCommand(GRIP, "grip", [&](ParseResult tokens) {
        unsigned long id = tokens.popPositiveInteger();
        grip(id);
      }, "Grips a jar with the given id.");
      addAvailableCommand(PICKUP, "pickup", [&](ParseResult tokens) {
        int id = tokens.popPositiveInteger();
        string locationName = tokens.popNoun();
        for (const Jar& jar : jars.items) {
          if (jar.id == id) {
            pickup(jar, locationName);
            return;
          }
        }
        cout << "Oups. No jar were found with this id." << endl;
      }, "Pickup a jar with the given id and location name.");
      addAvailableCommand(MOVE, "move", [&](ParseResult tokens) {
        char axis = tokens.popAxis();
        double dest = tokens.popScalaire();
        move(Movement(axis,dest));
      });
      addAvailableCommand(SWEEP, "sweep", [&](ParseResult tokens) {
        char axis = tokens.popAxis();
        double dest = tokens.popScalaire();
        move(Movement(axis,dest));
      });
      // m_commands["sweep"] = [&](ParseResult tokens) {sweep();};
      // m_commands["move"] = [&](ParseResult tokens) {
      // };
    
      // All lowercase
      // m_commands["grab"] = [&](ParseResult tokens) {
      //   double strength = tokens.popScalaire();
      //   cout << "Executing grab with strength = " << strength << endl;
      //   grab(strength);
      // };
      // 
      // m_commands["detect"] = [&](ParseResult tokens) {detect();};
      // m_commands["parse"] = [&](ParseResult tokens) {parse();};
      // m_commands["genloc"] = [&](ParseResult tokens) {generateLocations();}; // FIXME: The user should not be able to do this easily..
      // m_commands["pinpoint"] = [&](ParseResult tokens) {pinpoint();};
      // m_commands["calibrate"] = [&](ParseResult tokens) {calibrate();};
      // m_commands["putdown"] = [&](ParseResult tokens) {putdown();};
      // m_commands["fetch"] = [&](ParseResult tokens) {// Fetch an ingredient
      //   string ingredientName = tokens.popNoun();
      //   fetch(ingredientName);
      // }; 
      // 
      // m_commands["help"] = [&](ParseResult tokens) {
      //   // TODO
      // };
      // 
      // m_commands["cherche"] = [&](ParseResult tokens) { // fetch
      // };
      // 
      // m_commands["trouve"] = [&](ParseResult tokens) {
      // };
      // 
      // m_commands["rapporte"] = [&](ParseResult tokens) {
      // };

      // m_commands["capture"] = [&](ParseResult tokens) {capture();};

      // m_commands["photo"] = [&](ParseResult tokens) {
      //   Mat mat;
      //   captureFrame(mat);
      // };

      // m_commands["goto"] = [&](ParseResult tokens) {
      //   double x = tokens.popScalaire();
      //   double y = tokens.popScalaire();
      //   double t = tokens.popScalaire();
      //   moveTo(PolarCoord(x, y, t));
      // };
      // 

      // m_commands["open"] = [&](ParseResult tokens) {
      //   openJaw();
      // };
    
    }

    void waitUntilNotWorking() {
    }

    void reference() {
      cerr << "Doing reference...\n";
      //is_referenced = false; // Should set all axis to not referenced anymore?
      pushCommand(make_shared<ReferencingCommand>(axisR));
      pushCommand(make_shared<ReferencingCommand>(axisT));
      pushCommand(make_shared<ReferencingCommand>(axisH));
      move(Movement('t', CHANGE_LEVEL_ANGLE_HIGH));
      pushCommand(make_shared<ReferencingCommand>(axisV));
    }

    void home() {
      reference();
      gohome();
    }

    void gohome() {
      moveTo(PolarCoord(config.home_position_x, config.home_position_y, config.home_position_t));
      openJaw();
    }

    Axis axisH;
    Axis axisV;
    Axis axisT;
    Axis axisR;

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
        if (iequals(item.second.name, result.getCommand())) {
          item.second.func(result);
          found = true; break;
        }
      }
      if (!found) {
        cerr << "Error unkown command: " << result.getCommand();
      }

      if (pos != string::npos) {execute(str.substr(pos+1));}
    }

    void captureFrame(Mat& frame);
    void capture();
    void pickup(Jar jar, const string& locationName);

    void move(const std::vector<Movement>& mvts) {
      for (const Movement& mvt : mvts) {
        move(mvt);
      }
    }

    void move(const Movement& mvt) { // Deprecated, movement is deprecated, superseded by MoveCommand

      cerr << "Moving axis " << mvt.axis << " to " << mvt.destination << ".\n";
      Axis* axis = 0;
      if (mvt.axis == 'x' || mvt.axis == 'X') {
        axis = &axisH;
      } else if (mvt.axis == 'y' || mvt.axis == 'Y') {
        axis = &axisV;
      } else if (mvt.axis == 't' || mvt.axis == 'T') {
        axis = &axisT;
      }
      if (axis == 0) {return;}
      pushCommand(make_shared<MoveCommand>(*axis, mvt.destination));
    }

    void moveTo(PolarCoord destination) {
      pushCommand(make_shared<GotoCommand>(destination));
    }

    void grab(double strength) {
      pushCommand(make_shared<SlaveCommand>("g" + to_string(strength)));
    }

    void find(string ingredientName) {
    }

    // bring back to the working area
    void retreive() {
    }

    void sweep();
    void detect();
    void parse();
    void pinpoint();
    void calibrate();
    void putdown();
    void lowerForGrip(const Jar& jar); // Get lower, either to pickup, or to putdown
    void grip(int id);
    void grip(Jar jar);
    void store(const string& noun);
    void fetch(std::string ingredientName);

    void clearDetectedCodes() {
      db.clear(codes);
    }

    void pushCommand(shared_ptr<HedaCommand> cmd) {
      std::lock_guard<std::mutex> guard(commandsMutex);
      m_stack.push_back(cmd);
      calculatePendingCommands();
    }

    void stop() {
      std::lock_guard<std::mutex> guard(commandsMutex);
      m_stack.clear();
      m_pending_commands.clear();
      gripped_jar.id = -1;
      is_gripping = false;
      cout << "Executing stop" << endl;
      m_writer << "s";
      askPosition();
    }

    // Get the state of the arduino and set Heda with it. (I dont change arduino much now,
    // mainly Heda. So this avoids doing a reference every time I change something to Heda.)
    void sync() {
      std::lock_guard<std::mutex> guard(commandsMutex);
      askPosition();
    }

    void info() {
      m_writer << "?";
    }

    void openJaw() {
      pushCommand(make_shared<SlaveCommand>("r"));
    }

    void calculatePendingCommands() {
      m_pending_commands = "";
      // FIXME: Pretty print the commands
      /*for (const shared_ptr<HedaCommand>& cmd : m_stack) {
        m_pending_commands += m_commands.at(cmd->type).name;
        m_pending_commands += "\n";
      }*/
    }

    string getPendingCommands() {
      return m_pending_commands;
    }
    
    string getCurrentCommand() {
      // FIXME: Pretty print the commands
      return "";
      //if (m_stack.empty()) {return "";}
      //return m_commands.at(m_stack.front()->type).name;
    }

    // reference: What part of the arm is wanted to get the position? The tool? Which tool? The camera? etc
    // The x axis and the z axis are reversed
    // Warning: There are two solutions sometimes.
    // The offset is the offset between the two zeroes.
    PolarCoord toPolarCoord(const UserCoord c, double reference) {

      double z = -c.z + config.user_coord_offset_z;
      double t = (asin(z / reference) * 180.0 / PI);
      if (c.x > X_MIDDLE) { // FIXME: Is X_MIDDLE THE GOOD CONSTANT? Probably not. Use something from heda config.
        t = 180.0 - t;
      }
      double x = c.x - config.user_coord_offset_x + (cosd(t) * reference);

      double y = c.y - config.user_coord_offset_y;
      return PolarCoord(x, y, t);
    }

    double toUserHeight(const PolarCoord p) {
      return p.v+config.user_coord_offset_y;
    }

    // reference: What part of the arm is wanted to get the position? The tool? Which tool? The camera? etc
    UserCoord toUserCoord(const PolarCoord p, double reference) {

      double offsetX = config.user_coord_offset_x;
      double offsetY = config.user_coord_offset_y;
      double offsetZ = config.user_coord_offset_z;
      // The x axis and the z axis are reverse (hence * -1)
      return UserCoord(p.h + (cosd(p.t) * reference) + offsetX,
                  p.v+offsetY,
                  ((sind(p.t) * reference)*-1)+offsetZ);
    }

    UserCoord getCameraPosition() {
      return toUserCoord(m_position, config.camera_radius);
    }

    UserCoord getToolPosition() {
      return toUserCoord(m_position, config.gripper_radius);
    }
    
    PolarCoord getPosition() {
      return m_position;
    }

    /*Reader& getSlaveReader() {
      return m_reader;
    }
    
    Writer& getSlaveWriter() {
      return m_writer;
    }*/

//  protected:

    Reader& m_reader;
    Writer& m_writer;

    Writer& server_writer;

    LogWriter stack_writer;

    std::list<shared_ptr<HedaCommand>> m_stack;
    
    PolarCoord m_position;
    std::unordered_map<HedaCommandType, AvailableHedaCommand> m_commands;

    DetectedHRCodeTable codes;
    HedaConfigTable configs;
    ShelfTable shelves;
    JarFormatTable jar_formats;
    JarTable jars;
    IngredientTable ingredients;
    LocationTable locations;
    Database& db;

    bool isDoneWorking() {
      return m_stack.empty();
    }
    
    HedaConfig config;

    Jar gripped_jar;
    bool is_gripping = false;

    //void calculateGoto(vector<Movement> &movements, const PolarCoord position, const PolarCoord destination, std::function<void()> callback);
    // Does all the heavy logic. Breaks a movement into simpler movements and checks for collisions.
    void calculateGoto(vector<MoveCommand> &mvts, const PolarCoord destination);

    void generateLocations();

    // Returns the id of the self
    int shelfByHeight(double userHeight) {
    
      shelves.orderBy(shelfHeightCmp);
      auto previousIt = shelves.begin();
      for (auto it = shelves.begin(); it != shelves.end(); ++it) {
    
        if (userHeight < it->height) {return previousIt->id;}
        previousIt = it;
      }
     
      if (shelves.empty()) {return -1;}
      return shelves.back().id;
    }

    void runAllCommandStack() {
      while (!isDoneWorking()) {
        this_thread::sleep_for(chrono::milliseconds(handleCommandStack()));
      }
    }

    void loopCommandStack(Reader& reader) {
      while (true) {
        if(reader.inputAvailable()) {
          string cmd = getInputLine(reader);
          execute(cmd);
        }
        this_thread::sleep_for(chrono::milliseconds(handleCommandStack()));
      }
    }

    bool is_command_started = false;

    // returns the time to sleep
    int handleCommandStack() {

      std::lock_guard<std::mutex> guard(commandsMutex);

      if (m_stack.empty()) {return 100;}

      shared_ptr<HedaCommand>& current = *m_stack.begin();
      if (!is_command_started) {
        stack_writer << "Starting command: " << current->str();
        current->start(*this);
        is_command_started = true;
      }

      if (!current->isDone(*this)) {return 10;}

      stack_writer << "Finished command: " << current->str();
      current->doneCallback(*this);
      is_command_started = false;
      m_stack.pop_front();
      calculatePendingCommands();

      return 0;
    }

  protected:

    void askPosition() {
      
      // TODO: Assert already in this mutex. std::lock_guard<std::mutex> guard(commandsMutex);
      m_writer << "@";

      bool receivedMessagePosition = false;

      while (true) {
        if (m_reader.inputAvailable()) {
          string str = getInputLine(m_reader);

          if (str == MESSAGE_POSITION) {
            receivedMessagePosition = true; continue;

          } else if (receivedMessagePosition) {
            cout << "Parsing position = " << str << endl;
            Parser parser;
            ParseResult result;
            parser.parse(result, "foo " + str); // FIXME: useless command at beginning
            double x = result.popScalaire();
            double y = result.popScalaire();
            double t = result.popScalaire();
            m_position = PolarCoord(x, y, t);
            cout << "Done parsing position. Result: " << m_position << endl;
            return;
          }
        }
        this_thread::sleep_for(chrono::milliseconds(10));
      }
    }




    Shelf getWorkingShelf() {
      for (auto it = shelves.items.begin(); it != shelves.items.end(); it++) {
        if (it->id == config.working_shelf_id) {
          return *it;
        }
      }
      throw NoWorkingShelfException();
    }

    std::mutex commandsMutex;

    string m_pending_commands;

};

#endif
