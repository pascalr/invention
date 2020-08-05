#ifndef _READER_H
#define _READER_H

class GetByteOnEmptyStreamException : public exception {};

// TODO: The Reader should encapsulate to logic of detecting
// if a getByte has been called without a inputAvailable call.
// This would throw a GetByteBeforeInputAvailableException

class Reader {
  public:

    virtual bool inputAvailable() = 0;
    virtual int getByte() = 0;
    
};

#endif
