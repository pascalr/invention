#ifndef _SHARED_READER_H
#define _SHARED_READER_H

#include "reader.h"

using namespace std;

class GetByteOnEmptyStreamException : public exception {};

class SharedReader {
  public:
    
    SharedReader(Reader& reader) : m_reader(reader) {
    }

    bool inputAvailable(int clientId) {

      while (m_reader.inputAvailable()) {

        char ch = (char)m_reader.getByte();
        for (auto it : streams) {
          it.second += ch;
        }
      }
      
      string &stream = streams[clientId];
      return !stream.empty();
    }

    int getByte(int clientId) {

      string &stream = streams[clientId];

      if (stream.empty()) {throw GetByteOnEmptyStreamException();}

      char ch = stream[0];
      stream.erase(stream.begin());
      return ch;
    }

  protected:

    std::unordered_map<int, string> streams;
    Reader& m_reader;
    string m_input;
};

class SharedReaderClient : public Reader {
  public:
    SharedReaderClient(SharedReader &sharedReader, int id) : m_shared_reader(sharedReader), m_id(id) {
    }
    
    bool inputAvailable() {
      return m_shared_reader.inputAvailable(m_id);
    }
    
    int getByte() {
      return m_shared_reader.getByte(m_id);
    }

  protected:
    SharedReader &m_shared_reader;
    int m_id;
};

#endif
