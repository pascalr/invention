#include "test.h"

#include "../core/Database.h"
#include "../core/Model.h"

using namespace std;

class TestModel : public Model {
  public:
    TestModel() {}
    TestModel(double n, string w) : number(n), word(w) {}
    double number;
    string word;
};

class TestTable : public Table<TestModel> {
  public:
    const char* TABLE_NAME = "test_table";
    string getTableName() { return TABLE_NAME;}
    TestModel parseItem(SQLite::Statement& query) {
      TestModel model;
      model.number = query.getColumn(0);
      model.word = (const char *)query.getColumn(1);
      return model;
    }
    void bindQuery(SQLite::Statement& query, const TestModel& item) {
      query.bind(1, item.number);
      query.bind(2, item.word);
    }

};

void createTestTable() {
  Database db("data/unit_tests.db");
  db.execute("DROP TABLE IF EXISTS test_table");
  db.execute("CREATE TABLE IF NOT EXISTS test_table(id integer PRIMARY KEY AUTOINCREMENT NOT NULL, number REAL, word VARCHAR)");
}

void setup() {
}

void testClear() {
}

void testInsert() {
  title("testInsert");
  Database db("data/unit_tests.db");
  TestTable tests;
  db.load(tests);
  TestModel model = TestModel(11.11, "testInsert");
  db.insert(tests, model);
}

void testUpdate() {
  title("testUpdate");
  Database db("data/unit_tests.db");
  TestTable tests;
  db.load(tests);
  TestModel model = TestModel(22.22, "testUpdate");
  db.insert(tests, model);
  model.number = 33.33;
  db.update(tests, model);
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
  db.insert(jars, j);
}

void testDatabase() {
  title("testDatabase");

  clearJars();

  JarTable jars = loadJars();
  assertEqual("empty jars first", (unsigned)0, jars.items.size());

  insertOneJar(jars);
  assertEqual("one jar added", (unsigned)1, jars.items.size());
  
  jars = loadJars();
  assertEqual("one jar added (after reloading)", (unsigned)1, jars.items.size());
}

int main (int argc, char *argv[]) {

  signal(SIGINT, signalHandler);

  createTestTable();
  testInsert();
  testUpdate();
}
