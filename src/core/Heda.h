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

// Heda is the source of truth. It is the only class that should read the arduino serial and write to it.
// It has a stack of commands to execute.
// It keeps a copy of the serial it has read if it required at some point.
//
// TODO: Put everything I just put inside a class CommandStack.


class CommandStack {
};

class Heda {
  public:

    Heda(Writer& writer, Reader& reader) : m_reader(reader), m_writer(writer) {
    
      cerr << "Initializing video...\n";
      m_video_working = initVideo(m_cap);

      setupCommands();
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
        //m_command_stack.push_back([&](){reference();});
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
        m_writer << ss.str().c_str();
      };

      m_commands["open"] = [&](ParseResult tokens) {
        openJaw();
      };
    
    }

    void reference() {
      cerr << "Doing reference...\n";
      m_writer << "H";
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
      m_writer << mvt.str().c_str();

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

    void poll(string& s) {
      while (m_reader.inputAvailable()) {
        s += (char) m_reader.getByte();
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

    void stop() {
      m_writer << "s";
      clearStack();
      setIsWorking(false);
    }

    void info() {
      m_writer << "?";
    }

    void openJaw() {
      pushCommand("r");
    }

//  protected:

    Reader& m_reader;
    Writer& m_writer;

    std::list<string> m_command_stack;

    PolarCoord m_position;
    VideoCapture m_cap;
    std::unordered_map<string, std::function<void(ParseResult)>> m_commands;
    bool m_video_working = false;

    bool m_is_working = false;
};

#endif
