#ifndef MY_DESERIALIZE_H
#define MY_DESERIALIZE_H

#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/ptree.hpp>
using namespace boost::property_tree;

#include "program.h"
#include "axis.h"
#include "../config/constants.h"

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
