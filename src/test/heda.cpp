#include "test.h"

#include "../core/Heda.h"
#include "../core/writer/stream_writer.h"
#include "../core/reader/fake_reader.h"

using namespace std;

string popResult(Heda& heda, StreamWriter& ss, FakeReader& reader) {
  this_thread::sleep_for(chrono::milliseconds(200));
  string result = ss.str();
  reader.setFakeInput(MESSAGE_DONE);
  return result;
}

void testPosition() {
  title("testPosition");

  StreamWriter ss;
  FakeReader reader;
  Database db("data/test.db");
  Heda heda(ss, reader, db);

  heda.reference();
  reader.setFakeInput(MESSAGE_DONE);
  this_thread::sleep_for(chrono::milliseconds(100));
  PolarCoord pos; pos << HOME_POSITION_X, HOME_POSITION_Y, HOME_POSITION_Z;
  assertEqual("home position", pos, heda.getPosition());

  heda.move(Movement('x', 100.0));
  reader.setFakeInput(MESSAGE_DONE);
  this_thread::sleep_for(chrono::milliseconds(100));
  pos << 100, HOME_POSITION_Y, HOME_POSITION_Z;
  assertEqual("x 100", pos, heda.getPosition());

}

void testHomeCommand() {
  title("testHomeCommand");

  StreamWriter ss;
  FakeReader reader;
  Database db("data/test.db");
  Heda heda(ss, reader, db);

  heda.reference();
  this_thread::sleep_for(chrono::milliseconds(100));
  assertEqual("home", "H", ss.str());

  assertEqual("current command", "H", heda.getCurrentCommand());
  reader.setFakeInput(MESSAGE_DONE);
  this_thread::sleep_for(chrono::milliseconds(100));
  assertEqual("current command", "", heda.getCurrentCommand());

  heda.stop(); ss.str();
  heda.execute("goto 200 700 90");
  assertEqual("goto 200 700 90", "mt-6", popResult(heda, ss, reader));
  assertEqual("goto 200 700 90", "my700", popResult(heda, ss, reader));
  assertEqual("goto 200 700 90", "mt90", popResult(heda, ss, reader));
  assertEqual("goto 200 700 90", "mx200", popResult(heda, ss, reader));
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

  testPosition();
  testHomeCommand();
  //testSharedReader();
  //testCommandWriter();
}
