#ifndef LIB_H 
#define LIB_H

#include <string>
#include <exception>

void debug();

class StringMessageException : public std::exception {
  public:
    StringMessageException(std::string msg) : message(msg) {}
    virtual const char* what() const throw() {
      return message.c_str();
    }
    std::string message;
};

class EnsureException : public StringMessageException {
  public:
    EnsureException(std::string msg) : StringMessageException(msg) {}
};

class TimeoutException : public StringMessageException {
  public:
    TimeoutException(std::string msg) : StringMessageException(msg) {}
};

//template<typename T>
//void ensure(bool statement, T errorMessage);

void ensure(bool statement, std::string errorMessage);

// trim from start (in place)
void ltrim(std::string &s);

// trim from end (in place)
void rtrim(std::string &s);

// trim from both ends (in place)
void trim(std::string &s);

// https://stackoverflow.com/questions/2896600/how-to-replace-all-occurrences-of-a-character-in-string
std::string replaceAll(std::string str, const std::string& from, const std::string& to);

// insensitive equal
bool iequals(const std::string& a, const std::string& b);

#endif // COMMON_H
