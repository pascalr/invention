#include "test.h"

#include "../core/Heda.h"

using namespace std;

void setup() {
}

void testHomeCommand() {
  title("Testing home command");

  StringWriter str;
  Heda heda(str);
  heda.execute("home");
}

int main (int argc, char *argv[]) {

  signal(SIGINT, signalHandler);

  testHomeCommand();
}
