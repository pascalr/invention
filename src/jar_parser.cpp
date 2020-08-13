#include <iostream>
#include <stdio.h>
#include <string>
#include <chrono>
#include "core/jar_parser.h"
#include "core/Database.h"
#include "core/Model.h"

using namespace std;

int main(int argc, char** argv)
{
  Database db(DB_DEV);
  DetectedHRCodeTable codes;
  db.load(codes);

  for (const DetectedHRCode& code : codes.items) {
    parseJarCode(code);
    db.saveItem(codes, code);
  }

  return 0;
}
