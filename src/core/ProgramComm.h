#i_fndef _PROGRAM_COMM
#define _PROGRAM_COMM

#include "position.h"

class ProgramComm {
  public:

    // Init flags instead. INIT_ARDUINO | INIT_VIDEO, or INIT_ALL
    // INIT_ARDUINO = 00000001, INIT_VIDEO = 000000010, INIT_ALL = 111111111
    // Always init all for now.
    void init(bool initArduino = true, bool initVideo = true) {
      
      cerr << "Opening arduino port...";
      SerialPort m_port;
      if (m_port.openPort("/dev/ttyACM0") < 0) {
        throw "Error opening arduino port. Aborting...";
      }

      cerr << "Doing reference...";
      p.executeUntil("H", MESSAGE_DONE);

      if (!initVideo(m_cap)) {
        throw "ERROR! Unable to open camera\n";
      }
    }

    void detectOneCode() {
      vector<DetectedHRCode> detected;
      detect(frame, detected, RAYON, 0.0, 0.0, 0.0);
      if (detected.size() < 1) {
        throw "Error did not detect any HRCode. Aborting...";
        return -1;
      }
    }

    void captureFrame(Mat& frame) {
      // Maybe do 10 try with 0.01 second delay before throwing exception?
      m_cap.read(frame);
      if (frame.empty()) {
        throw "ERROR! blank frame grabbed\n";
      }
    }

    void moveTo(Vector3d destination) {
      string mvt = calculateMoveCommand(HOME_POSITION, destination);
      p.executeUntil(mvt, MESSAGE_DONE);
    }

    void grab(double strength) {
      p.executeUntil("G" + to_string(strength), MESSAGE_DONE);
    }

    void release() {
      p.executeUntil("H", MESSAGE_DONE);
    }

  protected:
    
    SerialPort m_port;
    VideoCapture m_cap;
};

#endif
