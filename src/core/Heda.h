#ifndef _HEDA_H
#define _HEDA_H

#include "position.h"
#include "../lib/serial.h"
#include "../lib/opencv.h"
#include "sweep.h" // because it is sweep that detects HRCodes

class HedaException : public exception {
};

class InitVideoException : public HedaException {};
class InitArduinoException : public HedaException {};
class MissingHRCodeException : public HedaException {};
class TooManyHRCodeException : public HedaException {};
class FrameCaptureException : public HedaException {};

class Heda {
  public:

    // Init flags instead. INIT_ARDUINO | INIT_VIDEO, or INIT_ALL
    // INIT_ARDUINO = 00000001, INIT_VIDEO = 000000010, INIT_ALL = 111111111
    // Always init all for now.
    void init(bool initArduinoFlag = true, bool initVideoFlag = true) {
      
      cerr << "Opening arduino port...";
      SerialPort m_port;
      if (m_port.openPort("/dev/ttyACM0") < 0) {
        throw InitArduinoException();
      }

      if (!initVideo(m_cap)) {
        throw InitVideoException();
      }
      
      reference();
    }

    void reference() {
      cerr << "Doing reference...";
      m_port.executeUntil("H", MESSAGE_DONE);
      m_position = HOME_POSITION;
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

    void captureFrame(Mat& frame) {

      for (int i = 0; i < 10; i++) {
        m_cap.read(frame);
        if (!frame.empty()) {return;}
      }
      throw FrameCaptureException();
    }

    void move(Movement mvt) {

      cerr << "Moving axis " << mvt.axis << " to " << mvt.destination << ".\n";
      m_port.executeUntil(mvt.str(), MESSAGE_DONE);

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
      m_port.executeUntil("G" + to_string(strength), MESSAGE_DONE);
    }

    void release() {
      m_port.executeUntil("H", MESSAGE_DONE);
    }

  protected:
    
    PolarCoord m_position;
    SerialPort m_port;
    VideoCapture m_cap;
};

#endif
