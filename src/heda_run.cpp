#include <iostream>
#include <stdio.h>
#include <string>
#include "core/heda.h"
#include "core/writer/serial_writer.h"
#include "core/reader/serial_reader.h"
#include <thread>
#include <chrono>

using namespace std::chrono;
using namespace cv;
using namespace std;

int main(int argc, char** argv)
{
  if (argc < 2) {
    cout << "Expected a command to be given as argument. Aborting..." << endl;
    return -1;
  }

  string cmd = argv[1];

  Database db(DB_DEV);

  SerialPort serialPort;
  if (serialPort.openPort("/tmp/heda0") < 0) {
    throw InitSerialPortException();
  }
  
  SerialReader serialReader(serialPort);
  SerialWriter serialWriter(serialPort);
  
  Heda heda(serialWriter, serialReader, db); 
  heda.execute(cmd);
  while (!heda.isDoneWorking()) {
    this_thread::sleep_for(chrono::milliseconds(50));
  }
  return 0;
}
