#ifndef COMMAND_WRITER
#define COMMAND_WRITER

#include <iostream>
#include "std_writer.h"
#include "serial_writer.h"

#include <mutex>

using namespace std;

std::mutex myMutex;

// CommandStackWriter?
class CommandWriter : public StdWriter {
  public:

    CommandWriter(SerialPort& p) : serial_writer(p) {

      thread writer_thread([&heda,&commands] {
        string s;
        while (true) {

          heda.poll(s);
          if (!s.empty()) {
                                                                             
            cout << "Arduino: " << s;
            ptree pt;
            stringstream ss;
            pt.put("log", s);
            json_parser::write_json(ss, pt);
                                                                             
            string str = ss.str();
            SimpleWeb::CaseInsensitiveMultimap header;
            header.emplace("Content-Length", to_string(str.length()));
            header.emplace("Content-Type", "application/json");
            response->write(SimpleWeb::StatusCode::success_ok, str, header);
            return;
          }
          this_thread::sleep_for(chrono::milliseconds(10));
        }
      });
      work_thread.detach();
    }

    void doPrint(string val) {
      std::lock_guard<std::mutex> guard(myMutex)
      commands.push_back(val);
    }

    std::list<string> commands;

    SerialWriter serial_writer;
};

#endif
