#ifndef HELPER_H
#define HELPER_H

#include <boost/log/core.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/sinks/text_file_backend.hpp>
#include <boost/log/utility/setup/file.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>
#include <boost/log/sources/severity_logger.hpp>
#include <boost/log/sources/record_ostream.hpp>

#include "../utils/io_common.h"

void setupInterrupt() {
  signal(SIGINT, signalHandler);
}

void setupLogging() {
  boost::log::add_file_log("log/main.log");

  boost::log::core::get()->set_filter(
    boost::log::trivial::severity >= boost::log::trivial::trace
  );

  boost::log::add_common_attributes();
}

#endif
