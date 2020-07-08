#ifndef SERIALIZE_HPP
#define SERIALOZE_HPP

#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/ptree.hpp>
using namespace boost::property_tree;

#include "program.h"

void deserialize(Program& p, const string& json_str) {
  ptree pt;
  read_json(json_str, pt);
  auto name = pt.get<string>("firstName") + " " + pt.get<string>("lastName");
}

#endif
