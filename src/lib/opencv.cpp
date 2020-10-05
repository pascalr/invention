#include "opencv.h"

#include <opencv2/opencv.hpp>
#include <opencv2/core.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/highgui.hpp>

using namespace std;
using namespace cv;

bool initVideo(VideoCapture& cap) {
  // cap.open(0) open the default camera using default API
  int deviceID = 0;             // 0 = open default camera
  //int apiID = cv::CAP_ANY;      // 0 = autodetect default API
  int apiID = cv::CAP_V4L2;      // 0 = autodetect default API
  
  cap.open(deviceID + apiID);
  if (!cap.isOpened()) {
    return false;
  }
  return true;
}

int captureVideoImage(Mat& frame) {
  VideoCapture cap;
  
  int deviceID = 0;             // 0 = open default camera
  int apiID = cv::CAP_V4L2;
  
  cap.open(deviceID + apiID);

  if (!cap.isOpened()) {
      cerr << "ERROR! Unable to open camera\n";
      return -1;
  }
  // wait for a new frame from camera and store it into 'frame'
  cap.read(frame);
  // check if we succeeded
  if (frame.empty()) {
    cerr << "ERROR! blank frame grabbed\n";
    return -1;
  }

  return 0;
}

int captureJpeg(vector<uchar> &encodeBuf) {
  Mat frame;
  int status = captureVideoImage(frame);
  if (status < 0) return status;
  imencode(".jpg",frame,encodeBuf);
  return status;
}
