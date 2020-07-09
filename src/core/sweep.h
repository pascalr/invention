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
#include "../helper/helper.h"
#include "../config/setup.h"
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

#define MAX_X AXIS_X_MAX_POS
#define MAX_Z AXIS_Z_MAX_POS

// Sends a command to arduino to move, then analyze in real time.
// When a new sticker appears, it asks the arduino what position it is.
// When a sticker disappers, it asks the arduino what position it is.
// It can then triangulate the real position of the 

class MovingDetectedCodes {
  public:
    double firstKnownPosition;
    double lastKnownPosition;
};


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
      m_port.writePort("?");
      m_port.waitUntilMessageReceived(MESSAGE_JSON);

      string programJson;
      m_port.waitUntilMessageReceived(programJson);

      StringWriter w;
      FakeProgram p(w);
      deserialize(p, programJson);

      Axis* axisX = axisByLetter(p.axes, 'X');
      Axis* axisZ = axisByLetter(p.axes, 'Z');

      x = axisX->getPosition();
      z = axisZ->getPosition();
    }

    void move(const char* txt, double pos, vector<Jar>& jars) {
      string str = txt;
      str += to_string((int)pos); // FIXME: Double should work too e.g. mx1.0
      cout << "Writing: " << str << endl;
      m_port.writePort(str);

      while (!m_port.messageReceived(MESSAGE_DONE)) {
        Mat frame;
        m_cap.read(frame);
        if (frame.empty()) {
          cerr << "ERROR! blank frame grabbed\n";
          this_thread::sleep_for(chrono::milliseconds(1));
          continue;
        }

        vector<HRCode> codes = detectHRCodes(frame);
        for (auto it = codes.begin(); it != codes.end(); it++) {
          HRCode code = *it;
          //jars.push_back();
          double x, z;
          askPosition(x, z);
          cerr << "FOUND: " << code << " at (" << x << ", " << z << ")" << endl;
        }
        this_thread::sleep_for(chrono::milliseconds(50));
      }
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
        move("MZ",0, jars);
        bool zUp = true; // Wheter the z axis goes from 0 to MAX or from MAX to 0

        move("MY",heights[i], jars);
        for (x = xUp ? 0 : MAX_X; xUp ? x <= MAX_X : x >= 0; x += xStep * (xUp ? 1 : -1)) {
          move("MX",x, jars);
          for (z = zUp ? 0 : MAX_Z; zUp ? z <= MAX_Z : z >= 0; z += zStep * (zUp ? 1 : -1)) {
            move("MZ",z, jars);
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
