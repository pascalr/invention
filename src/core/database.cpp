#include "database.h"

std::string quoteValue(std::string value) {
  return "\"" + value + "\"";
}
