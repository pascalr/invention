#include "test.h"

#include "../core/Heda.h"
#include "../core/writer/stream_writer.h"
#include "../core/reader/fake_reader.h"
#include "../core/reader/shared_reader.h"

using namespace std;

void setup() {
}

void testHomeCommand() {
  title("Testing home command");

  FakeReader reader;
  StreamWriter ss;
  Heda heda(ss, reader);
  heda.execute("home");
  assertEqual("home", "H", ss.str());

  heda.execute("grab 50");
  assertEqual("grab 50", "G50", ss.str());
}

void testSharedReader() {
  FakeReader src;
  SharedReader shared(src);
  SharedReaderClient dest1(shared, 1);
  SharedReaderClient dest2(shared, 2);
  src.setFakeInput("hello");
}

int main (int argc, char *argv[]) {

  signal(SIGINT, signalHandler);

  testHomeCommand();
}
