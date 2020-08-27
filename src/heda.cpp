#include <iostream>
#include <stdio.h>
#include <string>
#include "core/heda.h"
#include "core/writer/serial_writer.h"
#include "core/writer/console_writer.h"
#include "core/writer/log_writer.h"
#include "core/writer/log_writer.h"
#include "core/reader/serial_reader.h"
#include "core/reader/log_reader.h"
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
  
  LogWriter hedaLogWriter("\033[33mTo slave\033[0m", serialWriter);
  LogReader hedaLogReader("\033[34mFrom slave\033[0m", serialReader);

  LogWriter hedaOutputWriter("\033[37mTo server\033[0m");
  
  Heda heda(hedaLogWriter, hedaLogReader, db, hedaOutputWriter); 
  heda.sync();
  heda.execute(cmd);
  heda.runAllCommandStack();
  return 0;
}
