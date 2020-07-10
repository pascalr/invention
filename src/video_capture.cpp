#include "lib/opencv.h"
#include <iostream>
#include <stdio.h>

#include "lib/lib.h"

using namespace cv;
using namespace std;
int main(int, char**)
{
  signal(SIGINT, signalHandler);

  Mat frame;
  VideoCapture cap;
  if (!initVideo(cap)) {
    cerr << "ERROR! Unable to open camera\n";
    return -1;
  }

  cout << "Press any key to terminate" << endl;
  while (true) {
      cap.read(frame);
      if (frame.empty()) {
          cerr << "ERROR! blank frame grabbed\n";
          break;
      }

      // show live and wait for a key with timeout long enough to show images
      imshow("Live", frame);
      if (waitKey(40) >= 0)
          break;
  }
  return 0;
}
