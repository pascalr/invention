#include <iostream>
#include <stdio.h>
#include <string>
#include "core/Heda.h"
#include "core/writer/serial_writer.h"
#include "core/reader/serial_reader.h"

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
  return 0;
}

















/*
#include <wiringPi.h>
int main (void)
{
  wiringPiSetup () ;
  pinMode (0, OUTPUT) ;
  for (;;)
  {
    digitalWrite (0, HIGH) ; delay (500) ;
    digitalWrite (0,  LOW) ; delay (500) ;
  }
  return 0 ;
}
*/
