#ifndef _HEDA_H
#define _HEDA_H

#include "position.h"
#include "../lib/serial.h"
#include "../lib/opencv.h"
#include "sweep.h" // because it is sweep that detects HRCodes

#include "command_stack.h"

#include "parser.h"

class HedaException : public exception {};

class InitVideoException : public HedaException {};
class InitArduinoException : public HedaException {};
class MissingHRCodeException : public HedaException {};
class TooManyHRCodeException : public HedaException {};
class FrameCaptureException : public HedaException {};

#include <mutex>

std::mutex commandsMutex;

// Heda is the source of truth. It is the only class that should read the arduino serial and write to it.
// It has a stack of commands to execute.
// It keeps a copy of the serial it has read if it required at some point.

class Heda {
  public:

    Heda(Writer& writer, Reader& reader) : m_reader(reader), m_writer(writer) {
    
      cerr << "Initializing video...\n";
      m_video_working = initVideo(m_cap);

      setupCommands();

      m_commands_thread_run = true;
      m_commands_thread = std::thread(&Heda::loopCommandStack, this);
      //m_commands_thread.detach(); Do I need to detach?
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
      
      m_commands["stop"] = [&](ParseResult tokens) {
        cout << "Executing stop" << endl;
        stop();
      };
    
      m_commands["home"] = [&](ParseResult tokens) {
        reference();
      };

      m_commands["info"] = [&](ParseResult tokens) {
        info();
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
      pushCommand("H");
      m_position << HOME_POSITION_X, HOME_POSITION_Y, HOME_POSITION_Z;
    }

    void detectOneCode() {
      
      Mat frame;
      vector<DetectedHRCode> detected;
      detect(frame, detected, RAYON, 0.0, 0.0, 0.0);
      if (detected.size() < 1) {
        throw MissingHRCodeException();
      } else {
        throw TooManyHRCodeException();
      }
    }

    void execute(string cmd) {
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
    }

    void captureFrame(Mat& frame) {

      for (int i = 0; i < 10; i++) {
        m_cap.read(frame);
        if (!frame.empty()) {return;}
      }
      throw FrameCaptureException();
    }

    void move(Movement mvt) {

      cerr << "Moving axis " << mvt.axis << " to " << mvt.destination << ".\n";
      pushCommand(mvt.str());

      if (mvt.axis == 'x' || mvt.axis == 'X') {
        m_position(0) = mvt.destination;
      } else if (mvt.axis == 'y' || mvt.axis == 'Y') {
        m_position(1) = mvt.destination;
      } else if (mvt.axis == 't' || mvt.axis == 'T') {
        m_position(2) = mvt.destination;
      }
    }

    void moveTo(Vector3d destination) {

      vector<Movement> mvts;
      calculateMoveCommands(mvts, m_position, destination);
      for (auto it = mvts.begin(); it != mvts.end(); it++) {
        move(*it);
      }
    }

    void grab(double strength) {
      pushCommand("g"+to_string(strength));
    }

    void find(string ingredientName) {
    }

    // bring back to the working area
    void retreive() {
    }

    void sweep() {
    }

    void pushCommand(string str) {
      std::lock_guard<std::mutex> guard(commandsMutex);
      m_stack.push_back(str);
    }

    void stop() {
      std::lock_guard<std::mutex> guard(commandsMutex);
      m_writer << "s";
      m_stack.clear();
    }

    void info() {
      m_writer << "?";
    }

    void openJaw() {
      pushCommand("r");
    }

    string getCurrentCommand() {
      return m_current_command;
    }

//  protected:

    Reader& m_reader;
    Writer& m_writer;

    // This is the stack of raw commands sent to the arduino.
    // It is NOT the stack of commands sent to Heda.
    std::list<string> m_stack;
    
    std::thread m_commands_thread;
    std::atomic<bool> m_commands_thread_run;

    PolarCoord m_position;
    VideoCapture m_cap;
    std::unordered_map<string, std::function<void(ParseResult)>> m_commands;
    bool m_video_working = false;

    string m_current_command;

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
      if (m_current_command.empty()) {

        // Check if a new command is available
        if (m_stack.empty()) {return 100;}

        m_current_command = *m_stack.begin();
        m_writer << m_current_command.c_str();
        m_stack.pop_front();
        return 0;

      // Check if the message MESSAGE_DONE has been received.
      } else if (m_reader.inputAvailable()) {

        // TODO: This should only be one call to a reader function.
        string str;
        do {
          str += (char) m_reader.getByte();
        } while (m_reader.inputAvailable());

        if (str == MESSAGE_DONE) {
          m_current_command = "";
          return 0;
        }
      }

      return 10;
    }

};

#endif
