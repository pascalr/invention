#ifndef SWEEP_H
#define SWEEP_H

#include <iostream>
#include <algorithm>
#include <vector>
#include <zbar.h>

#include <opencv2/core.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/highgui.hpp>
#include <string>

#include "../utils/io_common.h"
#include "../lib/lib.h"
#include "../lib/hr_code.hpp"
#include "../lib/serial.h"
#include "../helper/logging.h"
#include "../config/setup.h"
#include "../config/constants.h"
#include "deserialize.h"
#include "fake_program.h"
#include "string_writer.h"

#include <chrono>
#include <thread>

using namespace std;
using namespace cv;
using namespace zbar;

typedef struct {
  string type;
  string data;
  vector <Point> location;
} DecodedObject;

class Jar {
  public:
    double position_x; // mm
    double position_y; // mm
    double position_z; // mm
};

void addUniqueToJars(vector<Jar> jars, vector<HRCodePosition> positions, double toolX, double toolZ, double angle) {

  double camDeltaX = (RAYON - CAMERA_OFFSET) * cos(angle / 180 * PI);
  double camDeltaZ = (RAYON - CAMERA_OFFSET) * sin(angle / 180 * PI);
  double camX = toolX - camDeltaX;
  double camZ = toolZ - camDeltaZ;

  // Field of view distance = 2 * distance * tan(FOV[angle] / 2)
  // Selon la loi du rapport des côtés d'un triangle b1/h1 = b2/h2 (triangle semblables)
  // (opposé sur adjacent egal opposé sur adjacent)
  // À une hauteur de 205mm, le code mesure ... de diamètre en pixel.
  // Si la hauteur double, le code mesure va diviser par 2.
  //
  // Le facteur de mouvement en x, y est proportionnel à la hauteur.
  //
  // ATTENTION: Je dois transférer les coordonées x,y et données x,z; Je dois tourner...

  for (auto pos = positions.begin(); pos != positions.end(); ++pos) {
    double scale = pos->scale;
    double imgDeltaX = pos->x - pos->originalImageWidth / 2.0 * scale;
    double imgDeltaY = pos->y - pos->originalImageHeight / 2.0 * scale;

    // x is side by side error, y is forward and backward error.
    //pos->y;
  }

}

#define MAX_X AXIS_X_MAX_POS
#define MAX_Z AXIS_Z_MAX_POS

// Sends a command to arduino to move, then analyze in real time.
// When a new sticker appears, it asks the arduino what position it is.
// When a sticker disappers, it asks the arduino what position it is.
// It can then triangulate the real position of the 

class Sweep {
  public:
    Sweep(SerialPort& p) : m_port(p) {
    }

    bool init() {
      if (!initVideo(m_cap)) {
        cerr << "ERROR! Unable to open camera. Aborting sweep.\n";
        return false;
      }
      return true;
    }

    void askPosition(double &x, double &z) {
      BOOST_LOG_TRIVIAL(debug) << "Asking position.";

      BOOST_LOG_TRIVIAL(debug) << "Writing ? to port.";
      m_port.writePort("?"); // FIXME: Clear buffer maybe
      BOOST_LOG_TRIVIAL(debug) << "Waiting until JSON message is received.";
      m_port.waitUntilMessageReceived(MESSAGE_JSON);

      BOOST_LOG_TRIVIAL(debug) << "Waiting until actual message is received.";
      string programJson;
      m_port.waitUntilMessageReceived(programJson);
      BOOST_LOG_TRIVIAL(debug) << "Waiting until DONE message is received.";
      m_port.waitUntilMessageReceived(MESSAGE_DONE);

      FakeProgram p;
      BOOST_LOG_TRIVIAL(debug) << "Deserializing " << programJson;
      deserialize(p, programJson);

      Axis* axisX = axisByLetter(p.axes, 'X');
      Axis* axisZ = axisByLetter(p.axes, 'Z');

      x = axisX->getPosition();
      z = axisZ->getPosition();
      
      BOOST_LOG_TRIVIAL(debug) << "Ask position done. x = " << x << ", z = " << z << ".";
    }

    void moveDirectly(const char* txt, double pos, vector<Jar> jars, double posX, double posY, double posZ) {
      string str = txt;
      str += to_string((int)pos); // FIXME: Double should work too e.g. mx1.0
      cout << "Writing: " << str << endl;
      BOOST_LOG_TRIVIAL(debug) << "About to move to: " << str;
      BOOST_LOG_TRIVIAL(debug) << "Locking serial port sweep.";
      m_port.lock(SERIAL_KEY_SWEEP);
      m_port.writePort(str);
      BOOST_LOG_TRIVIAL(debug) << "Waiting for message 'done'.";
      m_port.waitUntilMessageReceived(MESSAGE_DONE);
      BOOST_LOG_TRIVIAL(debug) << "Unlocking serial port sweep.";
      m_port.unlock();

      captureAndDetect(jars, posX, posY, posZ);
    }

    void captureAndDetect(vector<Jar>& jars, double posX, double posY, double posZ) {
      BOOST_LOG_TRIVIAL(debug) << "Capturing frame.";
      Mat frame;
      m_cap.read(frame);
      if (frame.empty()) {
        cerr << "ERROR! blank frame grabbed\n";
        return;
      }

      BOOST_LOG_TRIVIAL(debug) << "Trying to detect HR code positions.";
 
      HRCodeParser parser(0.2, 0.2);
      vector<HRCodePosition> positions;
      parser.findHRCodePositions(frame, positions, 100);

      if (!positions.empty()) {
        BOOST_LOG_TRIVIAL(debug) << "Positions detected.";
        //addUniqueToJars(m_jars, positions, posX, posZ);
      }
    }

    void move(const char* txt, double pos, vector<Jar>& jars) {
      string str = txt;
      str += to_string((int)pos); // FIXME: Double should work too e.g. mx1.0
      cout << "Writing: " << str << endl;
      BOOST_LOG_TRIVIAL(debug) << "About to move to: " << str;
      BOOST_LOG_TRIVIAL(debug) << "Locking serial port sweep.";
      m_port.lock(SERIAL_KEY_SWEEP);
      m_port.writePort(str);
      while (!m_port.messageReceived(MESSAGE_DONE)) {
        BOOST_LOG_TRIVIAL(debug) << "Capturing frame.";
        Mat frame;
        m_cap.read(frame);
        if (frame.empty()) {
          cerr << "ERROR! blank frame grabbed\n";
          this_thread::sleep_for(chrono::milliseconds(1));
          continue;
        }

        BOOST_LOG_TRIVIAL(debug) << "Trying to detect HR code positions.";
 
        HRCodeParser parser(0.2, 0.2);
        vector<HRCodePosition> positions;
        parser.findHRCodePositions(frame, positions, 100);

	      if (!positions.empty()) {
          BOOST_LOG_TRIVIAL(debug) << "Positions detected.";
          double x, z;
          askPosition(x, z);
          //addUniqueToJars(m_jars, positions, x, z);
          this_thread::sleep_for(chrono::milliseconds(500)); // To make sure it goes forward before stopping arduino again... Maybe "?100" so arduino runs 100 ms before returning position.
	      }
        
        this_thread::sleep_for(chrono::milliseconds(10));
      }
      BOOST_LOG_TRIVIAL(debug) << "Unlocking serial port sweep.";
      m_port.unlock();
    }

    void run(vector<Jar>& jars) {
      jars.clear();
      double heights[] = {0.0};
      //double xSweepIntervals[] = {0, 100, 200, 300, 400, 500, 600, 700, '\0'};
      double zStep = MAX_Z / 1;
      double xStep = MAX_X / 4;

      bool xUp = false; // Wheter the x axis goes from 0 to MAX or from MAX to 0

      double x = MAX_X;
      double z = 0;

      for (int i = 0; i < (sizeof(heights) / sizeof(double)); i++) {
        double y = heights[i];
        moveDirectly("MZ",0, jars, x, y, z);
        bool zUp = true; // Wheter the z axis goes from 0 to MAX or from MAX to 0

        moveDirectly("MY",heights[i], jars, x, y, z);
        for (x = xUp ? 0 : MAX_X; xUp ? x <= MAX_X : x >= 0; x += xStep * (xUp ? 1 : -1)) {
          moveDirectly("MX",x, jars, x, y, z);
          for (z = zUp ? 0 : MAX_Z; zUp ? z <= MAX_Z : z >= 0; z += zStep * (zUp ? 1 : -1)) {
            moveDirectly("MZ",z, jars, x, y, z);
            // Detect new ones and move to them to get exact position.
            // findBarCodes();
            
          }
          zUp = !zUp;
        }
        xUp = !xUp;
      }
    }

  private:
    VideoCapture m_cap;
    SerialPort& m_port;
};

#endif
