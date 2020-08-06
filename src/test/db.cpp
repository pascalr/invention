#include "test.h"

#include "../core/Database.h"
#include "../core/Model.h"

using namespace std;

void setup() {
}

void clearJars() {
  Database db("data/unit_tests.db");
  JarTable jars;
  db.clear(jars);
}

JarTable loadJars() {
  Database db("data/unit_tests.db");
  JarTable jars;
  db.load(jars);
  return jars;
}

void insertOneJar(JarTable& jars) {
  Database db("data/unit_tests.db");
  Jar j; j.position << 10, 10, 10;
  db.addItem(jars, j);
}

void testDatabase() {
  title("testDatabase");

  clearJars();

  JarTable jars = loadJars();
  assertEqual("empty jars first", 0, jars.items.size());

  insertOneJar(jars);
  assertEqual("one jar added", 1, jars.items.size());
  
  jars = loadJars();
  assertEqual("one jar added (after reloading)", 1, jars.items.size());
}

int main (int argc, char *argv[]) {

  signal(SIGINT, signalHandler);

  testDatabase();
}
