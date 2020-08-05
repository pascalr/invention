#ifndef _READER_H
#define _READER_H

class GetByteOnEmptyStreamException : public exception {};

class Reader {
  public:

    virtual bool inputAvailable() = 0;
    virtual int getByte() = 0;
    
};

#endif
