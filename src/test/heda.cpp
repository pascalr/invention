#include "test.h"

#include "../core/Heda.h"
#include "../core/writer/stream_writer.h"
#include "../core/reader/fake_reader.h"
#include "../core/reader/shared_reader.h"

using namespace std;

void setup() {
}

void testHomeCommand() {
  title("testHomeCommand");

  FakeReader reader;
  StreamWriter ss;
  Heda heda(ss, reader);
  heda.execute("home");
  assertEqual("home", "H", ss.str());

  heda.execute("grab 50");
  assertEqual("grab 50", "G50", ss.str());
}

void testSharedReader() {
  title("testSharedReader");

  FakeReader src;
  SharedReader shared(src);
  SharedReaderClient dest1(shared, 1);
  SharedReaderClient dest2(shared, 2);

  assertEqual("No input at first.", false, dest1.inputAvailable());

  src.setFakeInput("hello");
  
  assertEqual("Now there should be an input.", true, dest1.inputAvailable());
  assertEqual("h", 'h', (char) dest1.getByte());
  assertEqual("e", 'e', (char) dest1.getByte());
  assertEqual("h", 'h', (char) dest2.getByte());
}

int main (int argc, char *argv[]) {

  signal(SIGINT, signalHandler);

  testHomeCommand();
  testSharedReader();
}
