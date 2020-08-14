#include <iostream>
#include <stdio.h>
#include <string>
#include <chrono>
#include "core/jar_parser.h"
#include "core/Database.h"
#include "core/Model.h"
#include "core/jar_position_analyzer.h"

using namespace std;

// Step1: detect the position of the jar
// Step2: remove near duplicates
// Step3: parse the text
// Step4: if the text is not readable, store into data/negatives
// and later ask the user to translate

int main(int argc, char** argv)
{
  Database db(DB_DEV);
  DetectedHRCodeTable codes;
  db.load(codes);

  for (size_t i = 0; i < codes.items.size(); i++) {
    DetectedHRCode& code = codes.items[i];
    code.jar_id = "test";
    parseJarCode(code);
    db.update(codes, code);
  }

  return 0;
}
