#ifndef _STACK_H
#define _STACK_H

#include "reader/reader.h"
#include "writer/writer.h"
#include "writer/log_writer.h"

#include <list>
#include <mutex>

#define DONE_WORKING 0
#define STILL_WORKING 1
#define ERROR_OCCURED -1
#define EXCEPTION_OCCURED -2

class Stack {
  public:

    Stack() : stack_writer("\033[38;5;215mStack\033[0m") {}

    std::list<function<int()>> items;
    std::list<string> item_names;

    void push(string name, function<int()> func) {

      stack_writer << "Pushing command: " + name;
      std::lock_guard<std::mutex> guard(commandsMutex);
      items.push_back(func);
      item_names.push_back(name);
      //calculatePendingCommands();
    }

    void stop() {
      
      stack_writer << "Stopping. Clearing stack.";
      std::lock_guard<std::mutex> guard(commandsMutex);
      items.clear();
      item_names.clear();
      //m_pending_commands.clear();
    }

    /*void calculatePendingCommands(int level = 0) {
      m_pending_commands = "";
      for (const shared_ptr<HedaCommand>& cmd : m_stack) {
        m_pending_commands += cmd->allStr() + "\n";
      }
    }

    string getPendingCommands() {
      return m_pending_commands;
    }*/
    
    LogWriter stack_writer;
    
    bool empty() {
      return items.empty();
    }
    
    bool is_command_started = false;

    // returns the time to sleep
    int handle() {

      std::lock_guard<std::mutex> guard(commandsMutex);

      if (items.empty() || is_paused) {return 100;}

      function<int()>& current = *items.begin();
      if (!is_command_started) {
        stack_writer << "Starting command: " + *item_names.begin();
        is_command_started = true;
      }

      if (current() != DONE_WORKING) {return 10;}

      stack_writer << "Finished command: " + *item_names.begin();
      is_command_started = false;
      items.pop_front();
      item_names.pop_front();
      //calculatePendingCommands();

      return 0;
    }

    bool is_paused = false;
    
  protected:

    std::mutex commandsMutex;

    //string m_pending_commands;

};

#endif
