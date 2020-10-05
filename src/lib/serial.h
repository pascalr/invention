#ifndef SERIAL_H
#define SERIAL_H

#include <string>

// FIXME: Write error messages to a string instead of printf to stdout...

class SerialPort {

  public:

    ~SerialPort() {
      closePort();
    }

    void closePort();

    bool isOpen() {
      return m_is_opened;
    }

    // Device: "/dev/ttyACM0"
    // opens or repoens the given device
    int openPort(const char* device);

    void writePort(std::string str);

    void writePort(const char* str);

    int readBytes(char* buf, int length);

    int m_serial_port;
    bool m_is_opened = false;
};

#endif
