#ifndef COMMAND_WRITER
#define COMMAND_WRITER

#include <iostream>
#include "std_writer.h"
#include "serial_writer.h"
#include "../reader/shared_reader.h"

#include <mutex>

using namespace std;

std::mutex commandsMutex;

// CommandStackWriter?
class CommandWriter : public StdWriter {
  public:

    CommandWriter(SharedReader shared, SerialPort& p) : m_reader(shared, READER_CLIENT_ID_COMMAND_WRITER), m_port(p) {
    }

    void start() {
      thread command_thread([&] {
        while (true) {

          // Wait for a new command
          while (!commandAvailable()) {
            this_thread::sleep_for(chrono::milliseconds(10));
          }

          string command = popCommand();
          m_port.writePort(command);

          // Wait for the message MESSAGE_DONE to be received.
          while (!doneMessageReceived()) {
            this_thread::sleep_for(chrono::milliseconds(10));
          }
        }
      });
      command_thread.detach();
    }

    bool doneMessageReceived() {

      if (!m_reader.inputAvailable()) {return false;}

      string str;
      do {
        str += (char) m_reader.getByte();
      } while (m_reader.inputAvailable());

      cout << "CommandWriter: Received: " << str << endl;
      return str == MESSAGE_DONE;
    }

    string popCommand() {
      std::lock_guard<std::mutex> guard(commandsMutex);
      string str = *m_commands.begin();
      m_commands.pop_front();
      return str;
    }

    bool commandAvailable() {
      std::lock_guard<std::mutex> guard(commandsMutex);
      return !m_commands.empty();
    }

    void doPrint(string val) {
      std::lock_guard<std::mutex> guard(commandsMutex);
      m_commands.push_back(val);
    }

  protected:
    std::list<string> m_commands;

    SharedReaderClient m_reader;
    SerialPort& m_port;
};

#endif
