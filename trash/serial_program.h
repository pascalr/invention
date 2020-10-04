#ifndef _SERIAL_PROGRAM_H
#define _SERIAL_PROGRAM_H

#include "../utils/utils.h"
#include "../utils/io_common.h"
#include "program.h"
#include <thread>
#include <chrono>
#include "serial_writer.h"
#include "../lib/serial.h"

using namespace std;

class SerialProgram : public Program {
  protected:
    SerialWriter m_writer;
  public:
    SerialProgram() : Program(m_writer), m_writer(m_port) {

      if (m_port.openPort("/dev/pty8") < 0) {
        throw InitSerialPortException();
      }

    }

    Writer& getWriter() {
      return m_writer;
    }
    void setCurrentTime(unsigned long time) {
      currentTime = time;
    }

    void sleepMs(int time) {
      this_thread::sleep_for(chrono::milliseconds(time));
    }

    int getByte() {
      if (m_input.empty()) {
        m_port.lock(SERIAL_KEY_PROGRAM);
        m_port.getInput(m_input);
        m_port.unlock();
      }
      char c = m_input[0];
      m_input.erase(m_input.begin());
      return c;
    }

    bool getInput(char* buf, int size) {
      string str;
      m_port.lock(SERIAL_KEY_PROGRAM);
      m_port.getInput(str);
      m_port.unlock();

      if (((int) str.length()) >= size-1) {
        cerr << "Message is too long. Maximum: " << size << ". Was: " << str.length() << ".\n";
        return false;
      }

      strcpy(buf, str.c_str());
      rtrim(buf);
      return true;
    }

    bool inputAvailable() {
      m_port.lock(SERIAL_KEY_PROGRAM);
      bool inAvailable = m_port.inputAvailable();
      m_port.unlock();
      return inAvailable;
    }

    unsigned long getCurrentTime() {
      return currentTime;
    }

  protected:
    unsigned long currentTime = 0;
    
    SerialPort m_port;
    string m_input;
};

#endif
