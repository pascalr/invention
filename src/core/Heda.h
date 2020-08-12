#ifndef _HEDA_H
#define _HEDA_H

#include "position.h"
#include "../lib/serial.h"
#include "../lib/opencv.h"
#include "reader/reader.h"
#include "writer/writer.h"

#include "command_stack.h"

#include "parser.h"
#include "jar_packer.h"

#include "Database.h"

class HedaException : public exception {};

class InitVideoException : public HedaException {};
class InitArduinoException : public HedaException {};
class MissingHRCodeException : public HedaException {};
class TooManyHRCodeException : public HedaException {};
class FrameCaptureException : public HedaException {};

#include <mutex>

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

    Heda(Writer& writer, Reader& reader, Database &db) : m_reader(reader), m_writer(writer), db(db), m_packer(db) {
    
      cerr << "Initializing video...\n";
      m_video_working = initVideo(m_cap);

      setupCommands();

      m_commands_thread_run = true;
      m_commands_thread = std::thread(&Heda::loopCommandStack, this);
      //m_commands_thread.detach(); Do I need to detach?
      
      m_packer.setup();
    }

    ~Heda() {
      m_commands_thread_run = false;
      m_commands_thread.join();
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
      m_commands["home"] = [&](ParseResult tokens) {reference();};
      m_commands["info"] = [&](ParseResult tokens) {info();};
      m_commands["store"] = [&](ParseResult tokens) {store();};
      
      m_commands["help"] = [&](ParseResult tokens) {
        // TODO
      };

      m_commands["cherche"] = [&](ParseResult tokens) { // fetch
      };
      
      m_commands["trouve"] = [&](ParseResult tokens) {
      };
      
      m_commands["rapporte"] = [&](ParseResult tokens) {
      };

      m_commands["balaye"] = [&](ParseResult tokens) {
        sweep();
      };

      m_commands["goto"] = [&](ParseResult tokens) {
        double x = tokens.popScalaire();
        double y = tokens.popScalaire();
        double t = tokens.popScalaire();
        PolarCoord dest; dest << x, y, t;
        cout << "In goto\n";
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
      pushCommand("H", [&]() {
        m_position << HOME_POSITION_X, HOME_POSITION_Y, HOME_POSITION_Z;
      });
    }

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

    void captureFrame(Mat& frame) {

      for (int i = 0; i < 10; i++) {
        m_cap.read(frame);
        if (!frame.empty()) {return;}
      }
      throw FrameCaptureException();
    }

    void move(const std::vector<Movement>& mvts) {
      for (auto it = mvts.begin(); it != mvts.end(); it++) {
        move(*it);
      }
    }

    void move(const Movement& mvt) {

      cerr << "Moving axis " << mvt.axis << " to " << mvt.destination << ".\n";
      pushCommand(mvt.str(), [&,mvt]() {
        if (mvt.axis == 'x' || mvt.axis == 'X') {
          m_position(0) = mvt.destination;
        } else if (mvt.axis == 'y' || mvt.axis == 'Y') {
          m_position(1) = mvt.destination;
        } else if (mvt.axis == 't' || mvt.axis == 'T') {
          m_position(2) = mvt.destination;
        }
        if (mvt.callback) {mvt.callback();}
      });
    }

    void moveTo(PolarCoord destination) {

      cout << "WTF!\n";
      vector<Movement> mvts;
      cout << "Before!\n";
      calculateGoto(mvts, m_position, destination);
      cout << "Here!\n";
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

    // goto an empty place and drop the jar
    void store() {
      
      vector<Movement> mvts;
      m_packer.calculateStoreMovements();
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
    VideoCapture m_cap;
    std::unordered_map<string, std::function<void(ParseResult)>> m_commands;
    bool m_video_working = false;

    RawCommand m_current_command;
    
    DetectedHRCodeTable codes;
    Database& db;

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
              m_position << result.popScalaire(), result.popScalaire(), result.popScalaire();
            }
            this_thread::sleep_for(chrono::milliseconds(5));
          }
        }
      }

      return 10;
    }

    NaiveJarPacker m_packer;

    std::mutex commandsMutex;

    string m_pending_commands;
};

#endif
