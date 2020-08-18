#ifndef _HEDA_H
#define _HEDA_H

#include "position.h"
#include "../lib/serial.h"
#include "../lib/opencv.h"
#include "reader/reader.h"
#include "writer/writer.h"

#include "command_stack.h"

#include "parser.h"

#include "jar_parser.h"
//#include "jar_packer.h"

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

// RawCommand is bad. We should not be able to execute "mx..." this is too low level.
// The stack should be HedaCommand, which are object and not strings.
// A HedaCommand can be a Movement, a Detect, a Capture, etc...
// Utilies like sweep generates other HedaCommands
// Or they are done directrly by reading Heda source code.

// Low level command, sent to the arduino. "mx..." not "move x ..."
class RawCommand {
  public:
    RawCommand() {}
    RawCommand(std::string cmd) : cmd(cmd) {}
    RawCommand(std::string cmd, std::function<void()> callback) : cmd(cmd), callback(callback) {}

    bool isDone() {
      return cmd.empty();
    }
    void clear() {
      cmd = "";
    }
    void finish() {
      if (callback) {
        callback();
      }
      cmd = "";
    }
    std::string cmd;
    std::function<void()> callback;
};

// Heda is the source of truth. It is the only class that should read the arduino serial and write to it.
// It has a stack of commands to execute.
// It keeps a copy of the serial it has read if it required at some point.

class Heda {
  public:

    Heda(Writer& writer, Reader& reader, Database &db) : m_reader(reader), m_writer(writer), db(db)/*, m_packer(db)*/ {
    
      //cerr << "Initializing video...\n";
      //m_video_working = initVideo(m_cap);

      setupCommands();

      m_commands_thread_run = true;
      m_commands_thread = std::thread(&Heda::loopCommandStack, this);
      //m_commands_thread.detach(); Do I need to detach?
      
      //m_packer.setup();

      loadDb();
    }

    ~Heda() {
      m_commands_thread_run = false;
      m_commands_thread.join();
    }

    void loadDb() {
      db.load(configs);
      if (configs.empty()) {throw MissingConfigException();}
      config = *configs.begin();
      db.load(shelves, "ORDER BY height");
      db.load(codes);
      db.load(jar_formats);
      db.load(jars);
    }

    void setupCommands() {
     
      // at throws a out of range exception 
    
      // All lowercase
      m_commands["grab"] = [&](ParseResult tokens) {
        double strength = tokens.popScalaire();
        cout << "Executing grab with strength = " << strength << endl;
        grab(strength);
      };
      
      m_commands["stop"] = [&](ParseResult tokens) {stop();};
      m_commands["reference"] = [&](ParseResult tokens) {reference();};
      m_commands["home"] = [&](ParseResult tokens) {home();};
      m_commands["info"] = [&](ParseResult tokens) {info();};
      m_commands["store"] = [&](ParseResult tokens) {store();};
      m_commands["sweep"] = [&](ParseResult tokens) {sweep();};
      m_commands["balaye"] = [&](ParseResult tokens) {sweep();};
      m_commands["detect"] = [&](ParseResult tokens) {detect();};
      m_commands["parse"] = [&](ParseResult tokens) {parse();};
      m_commands["pinpoint"] = [&](ParseResult tokens) {pinpoint();};
      m_commands["calibrate"] = [&](ParseResult tokens) {calibrate();};
      m_commands["putdown"] = [&](ParseResult tokens) {putdown();};
      m_commands["grip"] = [&](ParseResult tokens) {
        unsigned long id = tokens.popPositiveInteger();
        grip(id);
      };
      
      m_commands["help"] = [&](ParseResult tokens) {
        // TODO
      };

      m_commands["cherche"] = [&](ParseResult tokens) { // fetch
      };
      
      m_commands["trouve"] = [&](ParseResult tokens) {
      };
      
      m_commands["rapporte"] = [&](ParseResult tokens) {
      };

      m_commands["capture"] = [&](ParseResult tokens) {capture();};

      m_commands["photo"] = [&](ParseResult tokens) {
        Mat mat;
        captureFrame(mat);
      };

      m_commands["goto"] = [&](ParseResult tokens) {
        double x = tokens.popScalaire();
        double y = tokens.popScalaire();
        double t = tokens.popScalaire();
        PolarCoord dest(x, y, t);
        moveTo(dest);
      };
      
      m_commands["move"] = [&](ParseResult tokens) {
        char axis = tokens.popAxis();
        double dest = tokens.popScalaire();
        stringstream ss; ss << "m" << axis << dest;
        pushCommand(ss.str());
      };

      m_commands["open"] = [&](ParseResult tokens) {
        openJaw();
      };
    
    }

    void waitUntilNotWorking() {
    }

    void reference() {
      cerr << "Doing reference...\n";
      is_referenced = false;
      pushCommand("hr");
      pushCommand("ht");
      pushCommand("hx");
      pushCommand("mt185"); // FIXME hardcoded
      pushCommand("hy", [&]() {
        m_position = PolarCoord(HOME_POSITION_X, HOME_POSITION_Y, HOME_POSITION_T);
        is_referenced = true;
      });
    }

    void home() {
      if (!is_referenced) {reference();}
      moveTo(PolarCoord(config.home_position_x, config.home_position_y, config.home_position_t));
      openJaw();
    }

    bool is_referenced = false;

    void execute(string str) {

      string::size_type pos = str.find('\n');
      string cmd = (pos == string::npos) ? str : str.substr(0, pos);

      cerr << "Executing cmd = " << cmd << "\n";

      string cmdName;
      Parser parser;
      ParseResult result;
      parser.parse(result, cmd);
      try {
        m_commands.at(result.getCommand())(result);
      } catch (const out_of_range &e) {
        // If the command does not exist, it throws an out of range exception.
      }

      if (pos != string::npos) {execute(str.substr(pos+1));}
    }

    void captureFrame(Mat& frame);
    void capture();

    void move(const std::vector<Movement>& mvts) {
      for (auto it = mvts.begin(); it != mvts.end(); it++) {
        move(*it);
      }
    }

    void move(const Movement& mvt) {

      cerr << "Moving axis " << mvt.axis << " to " << mvt.destination << ".\n";
      pushCommand(mvt.str(), [&,mvt]() {
        if (mvt.axis == 'x' || mvt.axis == 'X') {
          m_position.x = mvt.destination;
        } else if (mvt.axis == 'y' || mvt.axis == 'Y') {
          m_position.y = mvt.destination;
        } else if (mvt.axis == 't' || mvt.axis == 'T') {
          m_position.t = mvt.destination;
        }
        if (mvt.callback) {mvt.callback();}
      });
    }

    void moveTo(PolarCoord destination) {

      vector<Movement> mvts;
      calculateGoto(mvts, m_position, destination, doNothing);
      move(mvts);
    }

    void grab(double strength) {
      pushCommand("g"+to_string(strength));
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
    void grip(int id);

    void clearDetectedCodes() {
      db.clear(codes);
    }

    // goto an empty place and drop the jar
    void store() {
      
      vector<Movement> mvts;
      //m_packer.calculateStoreMovements();
      move(mvts);
    }

    void pushCommand(string str, std::function<void()> callback) {
      std::lock_guard<std::mutex> guard(commandsMutex);
      RawCommand cmd(str, callback);
      m_stack.push_back(cmd);
      calculatePendingCommands();
    }

    void pushCommand(string str) {
      std::lock_guard<std::mutex> guard(commandsMutex);
      RawCommand cmd(str);
      m_stack.push_back(cmd);
      calculatePendingCommands();
    }

    void stop() {
      std::lock_guard<std::mutex> guard(commandsMutex);
      cout << "Executing stop" << endl;
      m_writer << "s";
      m_stack.clear();
      m_pending_commands.clear();
      m_current_command.clear();
      m_writer << "@";
    }

    void info() {
      m_writer << "?";
    }

    void openJaw() {
      pushCommand("r");
    }

    void calculatePendingCommands() {
      m_pending_commands = m_current_command.cmd + "\n";
      for (const RawCommand& cmd : m_stack) {
        m_pending_commands += cmd.cmd;
        m_pending_commands += "\n";
      }
    }

    string getPendingCommands() {
      return m_pending_commands;
    }

    string getCurrentCommand() {
      return m_current_command.cmd;
    }

    // reference: What part of the arm is wanted to get the position? The tool? Which tool? The camera? etc
    // The x axis and the z axis are reversed
    // Warning: There are two solutions.
    // The offset is the offset between the two zeroes.
    // FIXME: Only the Y is updated, because I don't know how to handle the two solutions.
    PolarCoord toPolarCoord(const UserCoord c, double reference) {

      double offsetY = config.user_coord_offset_y;
      /*double offsetX = config.user_coord_offset_x;
      double offsetZ = config.user_coord_offset_z;
      double otherX = offsetX - c.x;
      double otherZ = offsetZ - c.z;
      double theta = asind(otherZ / reference);
      double deltaX = cos(theta) * reference;*/
      return PolarCoord(0.0, c.y - offsetY, 0.0);
    }

    double toUserHeight(const PolarCoord p) {
      return p.y+config.user_coord_offset_y;
    }

    // reference: What part of the arm is wanted to get the position? The tool? Which tool? The camera? etc
    UserCoord toUserCoord(const PolarCoord p, double reference) {

      double offsetX = config.user_coord_offset_x;
      double offsetY = config.user_coord_offset_y;
      double offsetZ = config.user_coord_offset_z;
      // The x axis and the z axis are reverse (hence * -1)
      return UserCoord(p.x + cosd(p.t) * reference + offsetX,
                  p.y+offsetY,
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

//  protected:

    Reader& m_reader;
    Writer& m_writer;

    // This is the stack of raw commands sent to the arduino.
    // It is NOT the stack of commands sent to Heda.
    std::list<RawCommand> m_stack;
    
    std::thread m_commands_thread;
    std::atomic<bool> m_commands_thread_run;

    PolarCoord m_position;
    //VideoCapture m_cap;
    std::unordered_map<string, std::function<void(ParseResult)>> m_commands;
    //bool m_video_working = false;

    RawCommand m_current_command;
    
    DetectedHRCodeTable codes;
    HedaConfigTable configs;
    ShelfTable shelves;
    JarFormatTable jar_formats;
    JarTable jars;
    Database& db;

    bool isDoneWorking() {
      return m_current_command.isDone() && m_stack.empty();
    }
    
    HedaConfig config;

    Jar gripped_jar;

    // Does all the heavy logic. Breaks a movement into simpler movements and checks for collisions.
    void calculateGoto(vector<Movement> &movements, const PolarCoord position, const PolarCoord destination, std::function<void()> callback);

    // Above which shelf is the arm on?
    int calculateLevel(double userHeight);

  protected:

    void loopCommandStack() {
      while (m_commands_thread_run) {
        this_thread::sleep_for(chrono::milliseconds(handleCommandStack()));
      }
    }

    // returns the time to sleep
    int handleCommandStack() {

      std::lock_guard<std::mutex> guard(commandsMutex);

      // Check if there is a command being executed right now
      if (m_current_command.isDone()) {

        // Check if a new command is available
        if (m_stack.empty()) { return 100;}

        m_current_command = *m_stack.begin();
        m_writer << m_current_command.cmd.c_str();
        m_stack.pop_front();
        calculatePendingCommands();
        return 0;

      // Check if the message MESSAGE_DONE has been received.
      } else if (m_reader.inputAvailable()) {

        string str = getInputLine(m_reader);

        if (str == MESSAGE_DONE) {
          m_current_command.finish();
          calculatePendingCommands();
          return 0;
        } else if (str == MESSAGE_POSITION) {
          while (true) {
            if (m_reader.inputAvailable()) {
              string pos = getInputLine(m_reader);
              Parser parser;
              ParseResult result;
              parser.parse(result, pos);
              m_position = PolarCoord(result.popScalaire(), result.popScalaire(), result.popScalaire());
            }
            this_thread::sleep_for(chrono::milliseconds(5));
          }
        }
      }

      return 10;
    }

    Shelf getWorkingShelf() {
      for (auto it = shelves.items.begin(); it != shelves.items.end(); it++) {
        if (it->id == config.working_shelf_id) {
          return *it;
        }
      }
      throw NoWorkingShelfException();
    }

    //NaiveJarPacker m_packer;

    std::mutex commandsMutex;

    string m_pending_commands;

};

#endif
