#ifndef SERIAL_H
#define SERIAL_H

// https://blog.mbedded.ninja/programming/operating-systems/linux/linux-serial-ports-using-c-cpp/

// C library headers
#include <stdio.h>
#include <string.h>

// Linux headers
#include <fcntl.h> // Contains file controls like O_RDWR
#include <errno.h> // Error integer and strerror() function
#include <termios.h> // Contains POSIX terminal control definitions
#include <unistd.h> // write(), read(), close()

// C++
#include <string>
#include <iostream>

#include "../lib/lib.h"
#include "../config/constants.h"
#include <thread>
#include <chrono>

#include <mutex>

std::mutex serialPortMutex;

class InitSerialPortException : public exception {};

// FIXME: Write error messages to a string instead of printf to stdout...

using namespace std;

class SerialPort {

  public:

    ~SerialPort() {
      closePort();
    }


    void closePort() {

      std::lock_guard<std::mutex> guard(serialPortMutex);

      if (m_is_opened) {
        close(m_serial_port);
        m_is_opened = false;
      }
    }

    bool isOpen() {

      std::lock_guard<std::mutex> guard(serialPortMutex);

      return m_is_opened;
    }

    // Device: "/dev/ttyACM0"
    // opens or repoens the given device
    int openPort(const char* device) {

      std::lock_guard<std::mutex> guard(serialPortMutex);

      // If already open, close first
      if (m_is_opened) {
        closePort();
      }
      // Open the serial port. Change device path as needed (currently set to an standard FTDI USB-UART cable type device)
      m_serial_port = open(device, O_RDWR);
      
      // Create new termios struc, we call it 'tty' for convention
      struct termios tty;
      memset(&tty, 0, sizeof tty);
      
      // Read in existing settings, and handle any error
      if(tcgetattr(m_serial_port, &tty) != 0) {
        printf("Error %i from tcgetattr: %s\n", errno, strerror(errno));
        return -1;
      }
      
      tty.c_cflag &= ~PARENB; // Clear parity bit, disabling parity (most common)
      tty.c_cflag &= ~CSTOPB; // Clear stop field, only one stop bit used in communication (most common)
      tty.c_cflag |= CS8; // 8 bits per byte (most common)
      tty.c_cflag &= ~CRTSCTS; // Disable RTS/CTS hardware flow control (most common)
      tty.c_cflag |= CREAD | CLOCAL; // Turn on READ & ignore ctrl lines (CLOCAL = 1)
      
      tty.c_lflag &= ~ICANON;
      tty.c_lflag &= ~ECHO; // Disable echo
      tty.c_lflag &= ~ECHOE; // Disable erasure
      tty.c_lflag &= ~ECHONL; // Disable new-line echo
      tty.c_lflag &= ~ISIG; // Disable interpretation of INTR, QUIT and SUSP
      tty.c_iflag &= ~(IXON | IXOFF | IXANY); // Turn off s/w flow ctrl
      tty.c_iflag &= ~(IGNBRK|BRKINT|PARMRK|ISTRIP|INLCR|IGNCR|ICRNL); // Disable any special handling of received bytes
      
      tty.c_oflag &= ~OPOST; // Prevent special interpretation of output bytes (e.g. newline chars)
      tty.c_oflag &= ~ONLCR; // Prevent conversion of newline to carriage return/line feed
      // tty.c_oflag &= ~OXTABS; // Prevent conversion of tabs to spaces (NOT PRESENT ON LINUX)
      // tty.c_oflag &= ~ONOEOT; // Prevent removal of C-d chars (0x004) in output (NOT PRESENT ON LINUX)
      
      tty.c_cc[VTIME] = 0;    // No blocking, return immediately with what is available
      tty.c_cc[VMIN] = 0;
      
      // Set in/out baud rate to be 9600
      cfsetispeed(&tty, B115200);
      cfsetospeed(&tty, B115200);
      
      // Save tty settings, also checking for error
      if (tcsetattr(m_serial_port, TCSANOW, &tty) != 0) {
        printf("Error %i from tcsetattr: %s\n", errno, strerror(errno));
        return -1;
      }

      // This definitely does not belongs here.
      //cerr << "Waiting until receiveing message ready.\n";
      //waitUntilMessageReceived(MESSAGE_READY); // TODO: timeout
      m_is_opened = true;

      return m_serial_port;
    }

    void getInput(string& str) {

      std::lock_guard<std::mutex> guard(serialPortMutex);

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

      std::lock_guard<std::mutex> guard(serialPortMutex);

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

    bool messageReceived(const char* msg) {

      if (!inputAvailable()) {
        return false;
      }

      string str;
      getInput(str);
      trim(str);
      cout << "Serial: Received: " << str << endl;
      return str == msg;
    }

    void waitUntilMessageReceived(const char* msg) {

      while (!messageReceived(msg)) {
        this_thread::sleep_for(chrono::milliseconds(10));
      }
    }

    void waitUntilMessageReceived(string& str) {

      while (!inputAvailable()) {
        this_thread::sleep_for(chrono::milliseconds(10));
      }
      
      getInput(str);
      trim(str);
    }

    void executeUntil(string str, string msg) {
      writePort(str);
      waitUntilMessageReceived(msg);
    }

    void writePort(string str) {
      writePort(str.c_str());
    }

    void writePort(const char* str) {

      std::lock_guard<std::mutex> guard(serialPortMutex);

      write(m_serial_port, str, strlen(str));
      write(m_serial_port, &m_separator, 1);
      cout << "Serial: Writing: " << str << endl;
    }

    void lock(int key) {
      while (m_key) {
        this_thread::sleep_for(chrono::milliseconds(10));
      }
      m_key = key;
    }

    void unlock() {
      m_key = 0;
    }
  
  private:
    int readBytes(char* buf, int length) {

      std::lock_guard<std::mutex> guard(serialPortMutex);

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

    int m_serial_port;
    char m_separator = '\n';
    string input;
    bool separatorFound = false;
    bool m_is_opened = false;
    int m_key = 0;
};

#endif
