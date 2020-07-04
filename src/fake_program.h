#ifndef FAKE_PROGRAM_H
#define FAKE_PROGRAM_H

#include "programme/utils.h"

using namespace std;

class FakeProgram : public Program {
  public:
    FakeProgram() : Program(m_writer) {
    }

    Writer& getWriter() {
      return m_writer;
    }
    unsigned long getCurrentTime() {
      return currentTime;
    }
    void setCurrentTime(unsigned long time) {
      currentTime = time;
    }

    void sleepMs(int time) {
      this_thread::sleep_for(chrono::milliseconds(time));
    }

    bool getInput(char* buf, int size) {
      string str;
      cin >> str;

      if (str.length() >= size-1) {
        cerr << "Message is too long. Maximum: " << size << ". Was: " << str.length() << ".\n";
        return false;
      }

      strcpy(buf, str.c_str());
      rtrim(buf);
      return true;
    }

    bool inputAvailable() {
      // https://stackoverflow.com/questions/6171132/non-blocking-console-input-c
      // we want to receive data from stdin so add these file
      // descriptors to the file descriptor set. These also have to be reset
      // within the loop since select modifies the sets.
      fd_set read_fds;
      FD_ZERO(&read_fds);
      FD_SET(STDIN_FILENO, &read_fds);
      int sfd = 1; // I think, because only 1 file descriptor
    
      struct timeval tv;
      tv.tv_sec = 0;
      tv.tv_usec = 0;
     
      int result = select(sfd + 1, &read_fds, NULL, NULL, &tv);
      if (result == -1 && errno != EINTR) {
        cerr << "Error in select: " << strerror(errno) << "\n";
      } else if (result == -1 && errno == EINTR) {
        exit(0); // we've received an interrupt - handle this
      } else {
        if (FD_ISSET(STDIN_FILENO, &read_fds)) {
          return true;
        }
      }
      return false;
    }

  protected:
    ConsoleWriter m_writer;
    unsigned long currentTime = 0;
};


#endif
