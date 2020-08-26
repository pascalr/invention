#ifndef _SHARED_READER_H
#define _SHARED_READER_H

#include "reader.h"
#include "serial_reader.h" // FIXME: Included for class GetByteOnEmptyStreamException 

#define READER_CLIENT_ID_HEDA 1
#define READER_CLIENT_ID_SERVER 2

using namespace std;

class UnregisteredClientException : public exception {};

// OPTIMIZE: I don't like the way this is implemented. You should call SharedReader::register,
// which returns a handle which the client uses. There should be no such thing as clientId.
// But this is far from a priority. This works so let's leave it at that.
class SharedReader {
  public:
    
    SharedReader(Reader& reader) : m_reader(reader) {
    }

    string &getStreamForClient(int clientId) {

      if (streams.empty()) {
        throw UnregisteredClientException();
      }

      auto stream = streams.find(clientId);
      if (stream == streams.end()) {
        throw UnregisteredClientException();
      }

      return stream->second;
    }

    void registerClient(int clientId) {
      streams[clientId] = "";
    }

    bool inputAvailable(int clientId) {

      while (m_reader.inputAvailable()) {

        char ch = (char)m_reader.getByte();
        for (std::unordered_map<int, string>::iterator it = streams.begin(); it != streams.end(); it++) {
          it->second = it->second + ch;
        }
      }
      
      string &stream = getStreamForClient(clientId);
      return !stream.empty();
    }

    int getByte(int clientId) {

      string &stream = getStreamForClient(clientId);

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
      sharedReader.registerClient(id);
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
