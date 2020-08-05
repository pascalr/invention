#include "test.h"

#include "../core/Heda.h"
#include "../core/writer/stream_writer.h"
#include "../core/reader/fake_reader.h"

using namespace std;

void setup() {
}

void testHomeCommand() {
  title("testHomeCommand");

  StreamWriter ss;
  FakeReader reader;
  Database db("data/test.db");
  Heda heda(ss, reader, db);

  heda.execute("home");
  this_thread::sleep_for(chrono::milliseconds(200));
  assertEqual("home", "H", ss.str());

  assertEqual("current command", "H", heda.getCurrentCommand());
  reader.setFakeInput(MESSAGE_DONE);
  this_thread::sleep_for(chrono::milliseconds(200));
  assertEqual("current command", "", heda.getCurrentCommand());

  heda.execute("grab 50");
  this_thread::sleep_for(chrono::milliseconds(200));
  assertEqual("grab 50", "G50", ss.str());
}

/*void testSharedReader() {
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
}*/

/*void testCommandWriter() {
  title("testCommandWriter");

  StreamWriter ss;
  FakeReader src;
  SharedReader shared(src);
  CommandWriter writer(shared, ss);

  assertEqual("Empty output at first", "", ss.str());
  assertEqual("Should not be working", false, writer.isWorking());
  writer << "h";
  this_thread::sleep_for(chrono::milliseconds(100));
  assertEqual("Should have written the first command", "h", ss.str());
  assertEqual("Should be working", true, writer.isWorking());
  
  writer << "e";
  assertEqual("It should not have written the second command (waiting for DONE)", "", ss.str());
  
  src.setFakeInput(MESSAGE_DONE);

  this_thread::sleep_for(chrono::milliseconds(100));
  assertEqual("Now it should have written the second output.", "e", ss.str());

}*/

int main (int argc, char *argv[]) {

  signal(SIGINT, signalHandler);

  testHomeCommand();
  //testSharedReader();
  //testCommandWriter();
}
