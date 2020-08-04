#ifndef _IO_READER_H
#define _IO_READER_H

#include "reader.h"

class IOReader : public Reader {
  public:

    bool inputAvailable() {
      return linuxInputAvailable();
    }

    int getByte() {
      char c;
      cin.get(c);
      return c;
    }

};

#endif
