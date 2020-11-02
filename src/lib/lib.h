#ifndef LIB_H 
#define LIB_H

#include <string>
#include <exception>
#include <vector>
#include <algorithm>

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

template <class T>
void order(std::vector<T>& items, double (*func)(const T&), bool ascending=true) {
  if (ascending) {
    std::sort(items.begin(), items.end(), [func](const T& arg0, const T& arg1){return func(arg0) < func(arg1);});
  } else {
    std::sort(items.rbegin(), items.rend(), [func](const T& arg0, const T& arg1){return func(arg0) < func(arg1);});
  }
}

//template<typename T>
//void ensure(bool statement, T errorMessage);

bool startsWith(std::string str, std::string prefix);
bool startsWith(const char *str, const char *ptr);

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
