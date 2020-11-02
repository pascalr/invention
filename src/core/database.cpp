#include "database.h"

const char* getEnvDefault(const char* name, const char* def) {
  char* r = std::getenv(name);
  return r == NULL ? def : r;
}

//std::string sanitizeQuote(std::string val) {
//  return sqlite3_mprintf("%Q", val.c_str());
//}
//
//std::string sanitize(std::string str) {
//  return sqlite3_mprintf("%q", str.c_str());
//}

