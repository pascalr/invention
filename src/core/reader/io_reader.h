#ifndef _IO_READER_H
#define _IO_READER_H

#include "reader.h"
#include "../../utils/io_common.h"

class IOReader : public Reader {
  public:

    bool inputAvailable() {

      if (hadInput) {return true;}

      return (hadInput = linuxInputAvailable());
    }

    int getByte() {
      char c;
      cin.get(c);
      if (c == '\n') {hadInput = false;}
      return c;
    }

    bool hadInput = false;

};

#endif
