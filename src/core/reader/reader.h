#ifndef _READER_H
#define _READER_H

// TODO: The Reader should encapsulate to logic of detecting
// if a getByte has been called without a inputAvailable call.
// This would throw a GetByteBeforeInputAvailableException
// But no because Arduino uses a reader (should it?) and it cannot have exceptions...

class Reader {
  public:

    virtual bool inputAvailable() = 0;
    virtual int getByte() = 0;
    
};

#ifndef ARDUINO

#include <string>
std::string getInputLine(Reader& r);

#endif

#endif
