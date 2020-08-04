#ifndef _FAKE_READER_H
#define _FAKE_READER_H

#include "reader.h"

class FakeReader : public Reader {
  public:

    void setFakeInput(const char* str) {
      fake_input.assign(str);
    }
    
    void setFakeInput(std::string& str) {
      fake_input.assign(str);
    }

        
    int getByte() {
      if (fake_input.empty()) {
        return -1;
      }
      int val = fake_input[0];
      fake_input.erase(fake_input.begin());
      return val;
    }
    
    bool inputAvailable() {
      return !fake_input.empty();
    }

    std::string fake_input;

};

#endif
