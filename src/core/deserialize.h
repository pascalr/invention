#ifndef MY_DESERIALIZE_H
#define MY_DESERIALIZE_H

#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/ptree.hpp>
using namespace boost::property_tree;

#include "program.h"
#include "axis.h"
#include "../config/constants.h"
#include "../helper/logging.h"

void deserialize(Axis* axis, ptree pt) {
  BOOST_LOG_TRIVIAL(debug) << "Parsing position steps.";
  axis->setPositionSteps(pt.get<long>(PROPERTY_POSITION));
  BOOST_LOG_TRIVIAL(debug) << "Parsing destination.";
  axis->setDestination(pt.get<double>(PROPERTY_DESTINATION));
  BOOST_LOG_TRIVIAL(debug) << "Parsing motor direction.";
  axis->setMotorDirection(pt.get<bool>(PROPERTY_FORWARD));
  BOOST_LOG_TRIVIAL(debug) << "Parsing is referencing.";
  axis->setIsReferencing(pt.get<bool>(PROPERTY_REFERENCING));
  BOOST_LOG_TRIVIAL(debug) << "Parsing is referenced.";
  axis->setIsReferenced(pt.get<bool>(PROPERTY_REFERENCED));
}

void deserialize(Program& p, const string& json_str) {
  ptree pt;
  std::istringstream is(json_str);
  BOOST_LOG_TRIVIAL(debug) << "About to read json.";
  read_json(is, pt);
  for (auto it: pt) {
    BOOST_LOG_TRIVIAL(debug) << "About get axis name.";
    char name = it.second.get<string>(PROPERTY_NAME)[0];
    Axis* axis = axisByLetter(p.axes, name);
    deserialize(axis, it.second);
  }
}

#endif
