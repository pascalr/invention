#ifndef _COMMAND_STACK_H
#define _COMMAND_STACK_H

/*#include <mutex>

std::mutex commandsMutex;

class CommandStack {
  public:

    void clearStack() {
      std::lock_guard<std::mutex> guard(commandsMutex);
      m_command_stack.clear();
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

    std::list<string> m_command_stack;
};*/

#endif 

