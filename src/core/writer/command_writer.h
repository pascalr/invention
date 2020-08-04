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

    CommandWriter(SharedReader& shared, Writer& writer) : m_reader(shared, READER_CLIENT_ID_COMMAND_WRITER), m_writer(writer) {
      start();
    }

    void start() {
      thread command_thread([&] {
        while (true) {

          if (m_is_working) {

            // Check if the message MESSAGE_DONE has been received.
            if (doneMessageReceived()) {
              setIsWorking(false);
            }  else {
              this_thread::sleep_for(chrono::milliseconds(10));
            }

          } else {

            // Wait for is a new command is available
            if (commandAvailable()) {

              string command = popCommand();
              m_writer << command.c_str();
              setIsWorking(true);
            } else {
              this_thread::sleep_for(chrono::milliseconds(100));
            }
          }
        }
      });
      command_thread.detach();
    }

    void setIsWorking(bool val) {
      std::lock_guard<std::mutex> guard(commandsMutex);
      m_is_working = val;
    }

    bool stopAll() {
      std::lock_guard<std::mutex> guard(commandsMutex);
      m_commands.clear();
      m_is_working = false;
    }

    bool isWorking() {
      return m_is_working;
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
    Writer& m_writer;
    bool m_is_working;
};

#endif
