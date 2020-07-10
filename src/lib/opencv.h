#ifndef MY_LIB_OPENCV_H
#define MY_LIB_OPENCV_H

#include <opencv2/opencv.hpp>
#include <opencv2/core.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/highgui.hpp>

using namespace std;
using namespace cv;

bool isCircle(vector<Point> contours, Point2f center, float radius, float epsilon) {
  // epsilon is a percentage of the radius
  // no more than 10% variation
  float maxVariation = radius * epsilon;
  bool isACircle = true;
  for( size_t i = 0; i < contours.size(); i++ ) {
    double norm = sqrt(pow(contours[i].x - center.x, 2)+pow(contours[i].y - center.y, 2));
    isACircle = isACircle && abs(norm-radius) < maxVariation;
  }
  return isACircle;
}

bool isBigCircle(vector<Point> contours, Point2f center, float radius, float epsilon, float minRadius) {
  return radius > minRadius && isCircle(contours, center, radius, epsilon);
}

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

#endif
