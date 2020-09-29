#ifndef _HEDA_H
#define _HEDA_H

class Heda;

#include "position.h"
#include "../lib/serial.h"
#include "../lib/opencv.h"
#include "reader/reader.h"
#include "writer/writer.h"
#include "writer/log_writer.h"

//#include "stack.h"

#include "parser.h"

#include "tess_parser.h"

#include "calibrate.h"

#include "database.h"

class HedaException : public exception {};

class InitArduinoException : public HedaException {};
class FrameCaptureException : public HedaException {};

class MissingConfigException : public exception {};
class NoWorkingShelfException : public exception {};

#include <mutex>

// TODO: Rename x and y to h and v at some point.
#define AXIS_H 'h'
#define AXIS_V 'v'
#define AXIS_T 't'
#define AXIS_R 'r'

void removeNearDuplicates(Heda& heda);
void parseCode(Heda& heda, DetectedHRCode& code);

class Axis {
  public:
    Axis(char id) : id(id) {}
    char id;
    bool is_referenced = false;
};

class HedaCommand {
  public:

    virtual string str() = 0;
    virtual string allStr(int level = 0) {
      string s;
      for (int i = 0; i < level; i++) {s += '-';}
      return s + str();
    }

    virtual void start(Heda& heda) = 0;

    virtual bool isDone(Heda& heda) {
      return true;
    }

    virtual void doneCallback(Heda& heda) {}
    
    virtual void setup(Heda& heda) {};
};

class LambdaCommand : public HedaCommand {
  public:
    LambdaCommand(std::function<void(Heda& heda)> func) : func(func) {}
    string str() {return "lambda";}
    void start(Heda& heda) {
      func(heda);
    }

    std::function<void(Heda& heda)> func;
};

using HedaCommandPtr = shared_ptr<HedaCommand>;

class SlaveCommand : public HedaCommand {
  public:
    
    SlaveCommand(string cmd) : cmd(cmd) {}

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

class OpenGripCommand : public SlaveCommand {
  public:
    OpenGripCommand() : SlaveCommand("r") {}
    string str() {return "open";}
    void doneCallback(Heda& heda);
};

class GrabCommand : public SlaveCommand {
  public:
    GrabCommand(double strength0) : SlaveCommand("g" + to_string(strength0)), strength(strength0) {}
    string str() {return "grab " + to_string(strength);}
    double strength;
};

class MoveCommand : public SlaveCommand {
  public:
    
    MoveCommand(Axis& axis0, double destination0) : SlaveCommand("m" + string(1, axis0.id) + to_string(destination0)), axis(axis0), destination(destination0) {
    }
    
    string str() {
      return "move " + string(1, axis.id) + " " + to_string(destination);
    }

    void doneCallback(Heda& heda);

    Axis& axis;
    double destination;
};

// Execute sub commands
class MetaCommand : public HedaCommand {
  public:

    string allStr(int level = 0) {
      string s;
      for (int i = 0; i < level; i++) {s += '-';}
      s += str();
      for (auto cmd : commands) {
        s += '\n' + cmd->allStr(level+1);
      }
      return s;
    }

    void start(Heda& heda);

    bool isDone(Heda& heda);

    virtual void setup(Heda& heda) = 0;

    void pushCommand(Heda& heda, shared_ptr<HedaCommand>& cmd);

  protected:

    unsigned int index = 0;
    vector<shared_ptr<HedaCommand>> commands;
};

class GripCommand : public MetaCommand {
  public:
    GripCommand(Jar jar) : jar(jar) {}
    string str() {return "grip " + to_string(jar.id);}
    void doneCallback(Heda& heda);
    void setup(Heda& heda);
    Jar jar;
};

class CloseupCommand : public MetaCommand {
  public:
    CloseupCommand(DetectedHRCode& code, Jar& jar) : detected(code), jar(jar) {}
    string str() {return "closeup " + to_string(detected.id);}
    void setup(Heda& heda);
    DetectedHRCode& detected;
    Jar& jar;
};

class LowerForGripCommand : public MetaCommand {
  public:
    LowerForGripCommand(Jar jar) : jar(jar) {}
    string str() {return "lowerforgrip " + to_string(jar.id);}
    void setup(Heda& heda);
    Jar jar;
};

class StoreCommand : public MetaCommand {
  public:
    StoreCommand(std::string name) : location_name(name) {}
    string str() {return "store " + location_name;}
    void doneCallback(Heda& heda);
    void setup(Heda& heda);
    std::string location_name;
    Location loc;
};

class PickupCommand : public MetaCommand {
  public:
    PickupCommand(Jar jar, Location loc) : jar(jar), loc(loc) {}
    string str() {return "pickup " + to_string(jar.id) + " " + loc.name;}
    void doneCallback(Heda& heda);
    void setup(Heda& heda);
    Jar jar;
    Location loc;
};

class HoverCommand : public MetaCommand {
  public:
    HoverCommand(double x, double z, double reference) : x(x), z(z), reference(reference) {} 

    string str() {return "hover " + to_string(x) + " " + to_string(z);}

  protected:
    void setup(Heda& heda);
    double x;
    double z;
    double reference;
};

class GotoCommand : public MetaCommand {
  public:
    GotoCommand(PolarCoord destination) : destination(destination) {}
    
    string str() {return "goto " + to_string(destination.h) + " " + to_string(destination.v) + " " + to_string(destination.t);}

  protected:
    PolarCoord destination;
    void setup(Heda& heda);
};

class PutdownCommand : public MetaCommand {
  public:
    string str() {return "putdown";}
    void setup(Heda& heda);
};

class ParseCodesCommand : public HedaCommand {
  public:
    string str() {return "parse";}
    void start(Heda& heda);
};

class PinpointCommand : public HedaCommand {
  public:
    string str() {return "pinpoint";}
    void start(Heda& heda);
};

class DetectCommand : public HedaCommand {
  public:
    string str() {return "detect";}
    void start(Heda& heda);
};

class StoreDetectedCommand : public MetaCommand {
  public:
    StoreDetectedCommand(DetectedHRCode code) : detected(code) {}
    string str() {return "stored " + to_string(detected.id);}
    void setup(Heda& heda);
    DetectedHRCode detected;
    Jar jar;
    Location loc;
    void doneCallback(Heda& heda);
};

class SweepCommand : public MetaCommand {
  public:
    string str() {return "sweep";}
    void setup(Heda& heda);
};

class UserAction : public HedaCommand {
  public:
    void start(Heda& heda);
    bool isDone(Heda& heda);
    virtual std::string getWaitingMessage() = 0;
    virtual std::string getActionRequired() = 0;
    void doneCallback(Heda& heda);
};

class ActionIdentify : public UserAction {
  public:
    ActionIdentify(int id) : id(id) {
    }
    string str() {return "actionNewJar";}
    string getWaitingMessage() {
      return "Un nouveau pot a été détecté. Pouvez-vous svp déterminer ses caractéristiques?";
    }
    string getActionRequired() {
      return "identify " + to_string(id);
    }
    int id;
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
              stack_writer("\033[38;5;215mStack\033[0m"),
              db(db) {
    
      loadDb();
    }

    void loadConfig() {
      db.load(configs);
      if (configs.empty()) {throw MissingConfigException();}
      config = *configs.begin();
    }

    void loadDb() {

      loadConfig();

      db.load(shelves);
      if (!shelves.get(working_shelf, config.working_shelf_id)) {
        throw NoWorkingShelfException();
      }

      db.load(codes);
      db.load(jar_formats);
      db.load(jars);
      db.load(locations);
      db.load(ingredients);
      db.load(units);
      db.load(recipes);
      db.load(ingredient_quantities);
    }

    void waitUntilNotWorking() {
    }

    Axis axisH;
    Axis axisV;
    Axis axisT;
    Axis axisR;

    void captureFrame(Mat& frame);
    void capture();
    //void move(const std::vector<Movement>& mvts) {
    //  for (const Movement& mvt : mvts) {
    //    move(mvt);
    //  }
    //}

    Axis* axisByName(char name) {
      Axis* axis = 0;
      if (name == 'h' || name == 'H') {
        axis = &axisH;
      } else if (name == 'v' || name == 'V') {
        axis = &axisV;
      } else if (name == 't' || name == 'T') {
        axis = &axisT;
      }
      return axis;
    }

    //void move(const Movement& mvt) { // Deprecated, movement is deprecated, superseded by MoveCommand

    //  cerr << "Moving axis " << mvt.axis << " to " << mvt.destination << ".\n";
    //  Axis* axis = 0;
    //  if (mvt.axis == 'h' || mvt.axis == 'H') {
    //    axis = &axisH;
    //  } else if (mvt.axis == 'v' || mvt.axis == 'V') {
    //    axis = &axisV;
    //  } else if (mvt.axis == 't' || mvt.axis == 'T') {
    //    axis = &axisT;
    //  }
    //  if (axis == 0) {return;}
    //  pushCommand(make_shared<MoveCommand>(*axis, mvt.destination));
    //}

    void moveTo(PolarCoord destination) {
      pushCommand(make_shared<GotoCommand>(destination));
    }

    void find(string ingredientName) {
    }

    void calibrate(JarFormat& format);

    void fetch(std::string ingredientName);

    void pushCommand(shared_ptr<HedaCommand> cmd) {

      stack_writer << "Pushing command: " + cmd->str();
      std::lock_guard<std::mutex> guard(commandsMutex);
      m_stack.push_back(cmd);
      //m_stack.back()->setup(*this);
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

    void calculatePendingCommands(int level = 0) {
      m_pending_commands = "";
      for (const shared_ptr<HedaCommand>& cmd : m_stack) {
        m_pending_commands += cmd->allStr() + "\n";
      }
    }

    string getPendingCommands() {
      return m_pending_commands;
    }
    
    double unitH(double unitX, double unitT, double reference) {
      return unitX - config.user_coord_offset_x + (cosd(unitT) * reference);
    }
    
    double unitX(double unitH, double unitT, double reference) {
      return unitH + config.user_coord_offset_x - (cosd(unitT) * reference); // Not tested
    }
    
    double unitZ(double unitT, double reference) {
      return config.user_coord_offset_z-(sind(unitT) * reference);
    }
    
    double unitV(double unitY) {
      return config.user_coord_offset_y - unitY;
    }

    double unitY(double unitV) {
      return config.user_coord_offset_y - unitV;
    }

    // reference: What part of the arm is wanted to get the position? The tool? Which tool? The camera? etc
    // The x axis and the z axis are reversed
    // Warning: There are two solutions sometimes.
    // The offset is the offset between the two zeroes.
    PolarCoord toPolarCoord(const UserCoord c, double reference) {

      double robotZ = -c.z + config.user_coord_offset_z;

      stringstream errorMsg; errorMsg << "toPolarCoord, wrong params, impossible to the given UserCoord, reference is too small. robotZ = ";
      errorMsg << robotZ << ", reference = " << reference;
      ensure(robotZ <= reference, errorMsg.str().c_str());
      
      double t = (asin(robotZ / reference) * 180.0 / PI);
      if (c.x > config.middleX()) { // FIXME: Is X_MIDDLE THE GOOD CONSTANT? Probably not. Use something from heda config.
        t = 180.0 - t;
      }
      double h = c.x - config.user_coord_offset_x + (cosd(t) * reference);

      return PolarCoord(h, unitV(c.y), t);
    }

    // reference: What part of the arm is wanted to get the position? The tool? Which tool? The camera? etc
    UserCoord toUserCoord(const PolarCoord p, double reference) {

      double offsetX = config.user_coord_offset_x;
      double offsetY = config.user_coord_offset_y;
      double offsetZ = config.user_coord_offset_z;
      // The x axis and the z axis are reverse (hence * -1)
      return UserCoord(p.h - (cosd(p.t) * reference) + offsetX,
                  offsetY-p.v,
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

    DetectedHRCodeTable codes;
    HedaConfigTable configs;
    ShelfTable shelves;
    JarFormatTable jar_formats;
    JarTable jars;
    IngredientTable ingredients;
    UnitTable units;
    RecipeTable recipes;
    LocationTable locations;
    IngredientQuantityTable ingredient_quantities;
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

    //void generateLocations();

    void shelfByHeight(Shelf& shelf, double userHeight) {

      if (shelves.empty()) {throw NoWorkingShelfException();}
    
      shelves.order(byHeight);
      auto previousIt = shelves.begin();
      for (auto it = shelves.begin(); it != shelves.end(); ++it) {
    
        if (userHeight < it->height) {shelf = *previousIt; return;}
        previousIt = it;
      }
     
      shelf = shelves.back();
      return;
    }

    void runAllCommandStack() {
      while (!isDoneWorking()) {
        this_thread::sleep_for(chrono::milliseconds(handleCommandStack()));
      }
    }

    bool is_command_started = false;

    // returns the time to sleep
    int handleCommandStack() {

      std::lock_guard<std::mutex> guard(commandsMutex);

      if (m_stack.empty() || is_paused) {return 100;}

      shared_ptr<HedaCommand>& current = *m_stack.begin();
      if (!is_command_started) {
        stack_writer << "Starting command: " + current->str();
        current->start(*this);
        is_command_started = true;
      }

      if (!current->isDone(*this)) {return 10;}

      stack_writer << "Finished command: " + current->str();
      current->doneCallback(*this);
      is_command_started = false;
      m_stack.pop_front();
      calculatePendingCommands();

      return 0;
    }

    Shelf working_shelf;

    bool is_paused = false;

    string waiting_message;
    string fatal_message;
    string action_required;

    string user_response;
    
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


    std::mutex commandsMutex;

    string m_pending_commands;

};

#endif
