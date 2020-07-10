#ifndef SWEEP_H
#define SWEEP_H

#include <iostream>
#include <algorithm>
#include <vector>
#include <zbar.h>

#include <string>

#include "../utils/io_common.h"
#include "../lib/lib.h"
#include "../lib/hr_code.hpp"
#include "../lib/serial.h"
#include "../lib/opencv.h"
#include "../helper/logging.h"
#include "../config/setup.h"
#include "../config/constants.h"
#include "deserialize.h"
#include "fake_program.h"

#include <chrono>
#include <thread>

using namespace std;
using namespace cv;
using namespace zbar;

class DetectedHRCodePosition {
  public:
    DetectedHRCodePosition(HRCodePosition pos, double x0, double y0, double z0) : position(pos), x(x0), y(y0), z(z0) {
    }
    HRCodePosition position;
    double x;
    double y;
    double z;
};
ostream &operator<<(std::ostream &os, const DetectedHRCodePosition &c) {
  return os << c.position << " at (" << c.x << ", " << c.y << ", " << c.z << ")";
}

class Jar {
  public:
    double position_x; // mm
    double position_y; // mm
    double position_z; // mm
};

void addUniqueToJars(vector<HRCodePosition> positions, double toolX, double toolZ, double angle) {

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

    void moveThanDetect(const char* txt, double pos, vector<DetectedHRCodePosition> &detected) {
      string str = txt;
      str += to_string(pos);
      //str += to_string((int)pos);
      BOOST_LOG_TRIVIAL(debug) << "About to move to: " << str;
      m_port.lock(SERIAL_KEY_SWEEP);
      m_port.writePort(str);
      BOOST_LOG_TRIVIAL(debug) << "Waiting for message 'done'.";
      m_port.waitUntilMessageReceived(MESSAGE_DONE);
      m_port.unlock();
      
      captureAndDetect(detected);
    }

    void captureAndDetect(vector<DetectedHRCodePosition> &detected) {
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
        for (auto it = positions.begin(); it != positions.end(); ++it) {
          DetectedHRCodePosition d(*it, m_x, m_y, m_z);
          detected.push_back(d);
        }
      }
    }

    void run(vector<DetectedHRCodePosition>& detected) {

      double heights[] = {0.0};
      //double xSweepIntervals[] = {0, 100, 200, 300, 400, 500, 600, 700, '\0'};
      double zStep = MAX_Z / 4;
      double xStep = MAX_X / 4;

      bool xUp = false; // Wheter the x axis goes from 0 to MAX or from MAX to 0

      m_x = MAX_X;
      m_z = 0.0;

      for (int i = 0; i < (sizeof(heights) / sizeof(double)); i++) {
        m_y = heights[i];
        moveThanDetect("MZ",0.0, detected);
        bool zUp = true; // Wheter the z axis goes from 0 to MAX or from MAX to 0

        moveThanDetect("MY",heights[i], detected);
        for (m_x = xUp ? 0.0 : MAX_X; xUp ? m_x <= MAX_X : m_x >= 0.0; m_x += xStep * (xUp ? 1 : -1)) {
          moveThanDetect("MX",m_x, detected);
          for (m_z = zUp ? 0.0 : MAX_Z; zUp ? m_z <= MAX_Z : m_z >= 0.0; m_z += zStep * (zUp ? 1 : -1)) {
            moveThanDetect("MZ",m_z, detected);
          }
          zUp = !zUp;
        }
        xUp = !xUp;
      }

      BOOST_LOG_TRIVIAL(debug) << "Sweep done. There are " << detected.size() << " possible candidates.";
    }

  private:
    VideoCapture m_cap;
    SerialPort& m_port;
    double m_x;
    double m_y;
    double m_z;
};

#endif
