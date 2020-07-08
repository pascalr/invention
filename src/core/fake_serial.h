// C library headers
#include <stdio.h>
#include <string.h>

// C++
#include <string>
#include <iostream>

#include <thread>

using namespace std;

class FakeSerialPort {

  public:

    ~FakeSerialPort() {
      closePort();
    }

    void closePort() {
      if (m_is_opened) {
        // TODO
        m_is_opened = false;
      }
    }

    bool isOpen() {
      return m_is_opened;
    }

    // Device: "/dev/ttyACM0"
    // opens or repoens the given device
    int openPort(const char* device) {
      // If already open, close first
      if (m_is_opened) {
        closePort();
      }

      thread m_work_thread([&p,response] {
        while (!p.inputAvailable()) {
          this_thread::sleep_for(chrono::milliseconds(10));
        }
        string s;
        p.getInput(s);
        cout << "Arduino: " << s;

        ptree pt;
        stringstream ss;
        pt.put("log", s);
        json_parser::write_json(ss, pt);

        response->write(ss.str());
      });
      m_work_thread.detach();
      
      setupAxes(&p.getWriter(), p.axes);

      m_is_opened = true;
      
      return 1;
    }

    void getInput(string& str) {
      if (separatorFound) {
        size_t i = input.find(m_separator);
        str.assign(input, 0, i+1);
        input.erase(0, i+1);
        if (input.find(m_separator) == string::npos) {
          separatorFound = false;
        }
      } else {
        cerr << "Error trying to getInput, but none was available." << endl;
      }
    }

    bool inputAvailable() {
      if (separatorFound) {
        return true;
      } else {
        char msg[256]; 
        memset(&msg, '\0', sizeof(msg));
        int num_bytes = readBytes(msg, sizeof(msg));
        if (num_bytes > 0) {
          input += msg;
          if (input.find(m_separator) != string::npos) {
            separatorFound = true;
          }
        }
      }
      return separatorFound;
    }
    
    void writePort(string str) {
      writePort(str.c_str());
    }

    void writePort(const char* str) {
      // TODO
    }
  
  private:
    int readBytes(char* buf, int length) {
      // Read bytes. The behaviour of read() (e.g. does it block?,
      // how long does it block for?) depends on the configuration
      // settings above, specifically VMIN and VTIME
      int num_bytes = read(m_serial_port, buf, length);
    
      // n is the number of bytes read. n may be 0 if no bytes were received, and can also be -1 to signal an error.
      if (num_bytes < 0) {
        printf("Error reading: %s", strerror(errno));
      }
      return num_bytes;
    }

    thread m_work_thread;
    FakeProgram m_program;
    char m_separator = '\n';
    string input;
    bool separatorFound = false;
    bool m_is_opened = false;
};

