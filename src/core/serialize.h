#ifndef MY_SERIALIZE_H
#define MY_SERIALIZE_H

#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/ptree.hpp>
using namespace boost::property_tree;

#include "program.h"
#include "axis.h"

template <typename T>
void writeJson(T& writer, const char* key, const char* val) {
  writer << "\"" << key << "\": \"" << val << "\", ";
}
template <typename T>
void writeJson(T& writer, const char* key, bool val) {
  writer << "\"" << key << "\": " << (val ? "true" : "false") << ", ";
}
template <typename T>
void writeJson(T& writer, const char* key, char val) {
  writer << "\"" << key << "\": \"" << val << "\", ";
}
template <typename T>
void writeJson(T& writer, const char* key, double val) {
  writer << "\"" << key << "\": " << val << ", ";
}
template <typename T>
void writeJson(T& writer, const char* key, int val) {
  writer << "\"" << key << "\": " << val << ", ";
}
template <typename T>
void writeJson(T& writer, const char* key, long val) {
  writer << "\"" << key << "\": " << val << ", ";
}
template <typename T>
void writeJson(T& writer, const char* key, unsigned long val) {
  writer << "\"" << key << "\": " << val << ", ";
}

template <typename T>
void serialize(Program& p, T& out) {
  out << "{";
  for (int i = 0; p.axes[i] != NULL; i++) {
    out << "\"axis_" << p.axes[i]->name << "\": ";
    serialize(p.axes[i], out);
    if (p.axes[i+1] != NULL) {
      out << ", ";
    }
  }
  out << "}";
}

#define PROPERTY_NAME "name"
#define PROPERTY_POSITION "pos"
#define PROPERTY_DESTINATION "dest"
#define PROPERTY_FORWARD "forward"
#define PROPERTY_REFERENCED "referenced"
#define PROPERTY_REFERENCING "referencing"
#define PROPERTY_SPEED "speed"

template <typename T>
void serialize(Axis* axis, T& out) {
  out << "{";
  writeJson(out, PROPERTY_NAME, axis->name);
  writeJson(out, PROPERTY_POSITION, axis->getPositionSteps());
  writeJson(cout, PROPERTY_POSITION, axis->getPositionSteps());
  writeJson(out, PROPERTY_DESTINATION, axis->getDestination());
  writeJson(out, PROPERTY_FORWARD, axis->isForward);
  writeJson(out, PROPERTY_REFERENCED, axis->isReferenced);
  writeJson(out, PROPERTY_REFERENCING, axis->isReferencing);
  out << "\"" << PROPERTY_SPEED << "\": " << axis->speed; 
  // CAREFULL: JSON MUST NOT END WITH COMMA
  out << "}";
}

void deserialize(Axis* axis, ptree pt) {
  axis->setPositionSteps(pt.get<long>(PROPERTY_POSITION));
  axis->setDestination(pt.get<double>(PROPERTY_DESTINATION));
  axis->setMotorDirection(pt.get<bool>(PROPERTY_FORWARD));
  axis->setIsReferencing(pt.get<bool>(PROPERTY_REFERENCING));
  axis->setIsReferenced(pt.get<bool>(PROPERTY_REFERENCED));
}

void deserialize(Program& p, const string& json_str) {
  ptree pt;
  std::istringstream is(json_str);
  read_json(is, pt);
  for (auto it: pt) {
    char name = it.second.get<string>(PROPERTY_NAME)[0];
    Axis* axis = axisByLetter(p.axes, name);
    deserialize(axis, it.second);
  }
}

#endif
