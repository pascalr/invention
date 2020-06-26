#include <opencv2/opencv.hpp>
#include <opencv2/core.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/highgui.hpp>
#include <iostream>
#include <stdio.h>
#include <string>
#include <chrono>

#include "common.h"

//#include <filesystem>
//namespace fs = std::filesystem;
//fs::exists("capture.cpp");

using namespace std::chrono;
using namespace cv;
using namespace std;

int main(int argc, char** argv)
{
  string outfile_name = "output/";
  if ( argc == 2 ) {
    outfile_name += argv[1];
  } else {
    std::stringstream ss;
    for (int i = 1; i < 1000; i++) {
      ss << "capture_" << std::setw(3) << std::setfill('0') << i << ".jpg";
      if (!file_exists(ss.str().c_str())) break;
    }
    outfile_name += ss.str();
  }/* else {
    system_clock::time_point now = std::chrono::system_clock::now();
    time_t t_c = std::chrono::system_clock::to_time_t(now);
    std::stringstream ss;
    ss << "capture_" << put_time(std::localtime(&t_c), "%F_%T") << ".jpg";
    outfile_name += ss.str();
  }*/
  cout << outfile_name << endl;

  Mat frame;
  captureVideoImage(frame);
  imwrite(outfile_name, frame);
  imshow("Live", frame);
  waitKey();
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
