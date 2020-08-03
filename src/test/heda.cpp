#include "test.h"

#include "../core/Heda.h"
#include "../core/stream_writer.h"

using namespace std;

void setup() {
}

void testHomeCommand() {
  title("Testing home command");

  StreamWriter ss;
  Heda heda(ss);
  heda.execute("home");

  assertEqual("home", "H", ss.str());
}

int main (int argc, char *argv[]) {

  signal(SIGINT, signalHandler);

  testHomeCommand();
}
