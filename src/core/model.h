#ifndef _MODEL_H
#define _MODEL_H

#include "SQLiteCpp/SQLiteCpp.h"
#include <vector>

class WrongModelTypeException : public std::exception {};

template<typename T>
std::string getTableName();

// TODO: Validate column names indexes

template <class T>
int byJarId(const T& t) {
  return t.jar_id;
}
template <class T>
std::string byName(const T& t) {
  return t.name;
}
template <class T>
double byHeight(const T& t) {
  return t.height;
}
template <class T>
double byLidY(const T& t) {
  return t.lid_coord.y;
}
//template <class T>
//void order(std::vector<T>& items, double (*func)(const T&), bool ascending=true) {
//  if (ascending) {
//    std::sort(items.begin(), items.end(), [func](const T& arg0, const T& arg1){return func(arg0) < func(arg1);});
//  } else {
//    std::sort(items.rbegin(), items.rend(), [func](const T& arg0, const T& arg1){return func(arg0) < func(arg1);});
//  }
//}

class Model {
  public:
    Model() {
      time_t time; localtime(&time);
      created_at = time;
      updated_at = time;
    }
    long id = -1;
    time_t created_at;
    time_t updated_at;

    bool exists() {
      return id != -1;
    }
    
    // This could be bad because of implicit conversion to other types from what I've read.
    //operator bool() const {
    //  return id == -1;
    //}
};

#endif
