#ifndef _COMMAND_STACK_H
#define _COMMAND_STACK_H

#include <mutex>

std::mutex commandsMutex;

class CommandStack {
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

    void clearStack() {
      std::lock_guard<std::mutex> guard(commandsMutex);
      m_command_stack.clear();
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
      string str = *m_command_stack.begin();
      m_command_stack.pop_front();
      return str;
    }

    bool commandAvailable() {
      std::lock_guard<std::mutex> guard(commandsMutex);
      return !m_command_stack.empty();
    }

    void pushCommand(string val) {
      std::lock_guard<std::mutex> guard(commandsMutex);
      m_command_stack.push_back(val);
    }
};

#endif 

