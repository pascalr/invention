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

  heda.execute("grab 50");
  assertEqual("grab 50", "G50", ss.str());
}

int main (int argc, char *argv[]) {

  signal(SIGINT, signalHandler);

  testHomeCommand();
}
