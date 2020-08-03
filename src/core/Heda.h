#ifndef _HEDA_H
#define _HEDA_H

#include "position.h"
#include "../lib/serial.h"
#include "../lib/opencv.h"
#include "sweep.h" // because it is sweep that detects HRCodes

#include "parser.h"

class HedaException : public exception {};

class InitVideoException : public HedaException {};
class InitArduinoException : public HedaException {};
class MissingHRCodeException : public HedaException {};
class TooManyHRCodeException : public HedaException {};
class FrameCaptureException : public HedaException {};


class Heda {
  public:

    Heda(Writer& writer) : m_writer(writer) {
    
      cerr << "Initializing video...\n";
      m_video_working = initVideo(m_cap);

      setupCommands();
    }

    void setupCommands() {
     
      // at throws a out of range exception 
    
      // All lowercase
      m_commands["grab"] = [&](vector<Token> tokens) {
        cout << "Executing grab with strength = " << tokens.at(0).toScalaire() << endl;
        grab(tokens.at(0).toScalaire());
      };
      
      m_commands["stop"] = [&](vector<Token> tokens) {
        cout << "Executing stop" << endl;
        stop();
      };
    
      m_commands["home"] = [&](vector<Token> tokens) {
        reference();
      };
    
      /*Parser parser;
      parser.keywords["t."] = Volume(1);
      parser.keywords[""] = ...
      Quantity("");
      sscanf
      sscanf (sentence,"%s %*s %d",str,&i);
    
      Command("grab", "^%d$");
      Command("release");
      Command("move", "^%d %d %d$");*/
    
      // moveX
      // moveT
      // moveZ?
      // moveY
    
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
      vector<Token> tokens;
      parse(cmdName, tokens, cmd);
      try {
        m_commands.at(cmdName)(tokens);
      } catch (out_of_range e) {
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

    void poll() {
      /*server.resource["^/poll$"]["GET"] = [&p](shared_ptr<HttpServer::Response> response, shared_ptr<HttpServer::Request> request) {
        if (p.isOpen()) {
          thread work_thread([&p,response] {
            while (true) {
              p.lock(SERIAL_KEY_POLL);
              if (p.inputAvailable()) {
                string s;
                p.getInput(s);
                p.unlock();
                cout << "Arduino: " << s;
                                                                                 
                ptree pt;
                stringstream ss;
                pt.put("log", s);
                json_parser::write_json(ss, pt);
                                                                                 
                string str = ss.str();
                SimpleWeb::CaseInsensitiveMultimap header;
                header.emplace("Content-Length", to_string(str.length()));
                header.emplace("Content-Type", "application/json");
                response->write(SimpleWeb::StatusCode::success_ok, str, header);
                return;
              }
              p.unlock();
              this_thread::sleep_for(chrono::milliseconds(10));
            }
          });
          work_thread.detach();
        }
      };*/
    }

    void grab(double strength) {
      m_writer << "G" << strength;
    }

    void sweep() {
    }

    void stop() {
      m_writer << "s";
    }

    void release() {
      m_writer << "r";
    }

//  protected:

    Writer& m_writer;
    bool m_working;

    PolarCoord m_position;
    VideoCapture m_cap;
    std::unordered_map<string, std::function<void(vector<Token>)>> m_commands;
    bool m_video_working = false;
};

#endif
