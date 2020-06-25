#ifndef COMMON_H
#define COMMON_H

#include <opencv2/opencv.hpp>
#include <opencv2/core.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/highgui.hpp>

using namespace cv;
using namespace std;

int captureVideoImage(Mat& frame) {
  VideoCapture cap;
  
  int deviceID = 0;             // 0 = open default camera
  int apiID = cv::CAP_ANY;      // 0 = autodetect default API
  
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

// TODO: Test distances too, not just size.
bool isValidHRCode(int i, int child, int firstInnerCircle, int secondInnerCircle, vector<Point2f> centers, vector<float> radius) {
  // All relatives because actual size vary based on distance of camera to sticker.
  float borderThickness = 2; // mm
  float insideDiameter = 30; // mm
  float smallDotDiameter = 4.86; // mm
  float outsideDiameter = insideDiameter + 2*borderThickness; // mm

  float expectedRatio = outsideDiameter / insideDiameter;
  float actualRatio = radius[i] / radius[child];
  bool perimeterDetected = abs(actualRatio - expectedRatio)/expectedRatio < 0.2;

  float dotExpectedRatio = outsideDiameter / smallDotDiameter;
  float dotActualRatio = radius[i] / radius[firstInnerCircle];
  bool dotDetected = abs(dotActualRatio - dotExpectedRatio)/dotExpectedRatio < 0.2;

  float dotActualRatio2 = radius[i] / radius[secondInnerCircle];
  bool dotDetected2 = abs(dotActualRatio2 - dotExpectedRatio)/dotExpectedRatio < 0.2;

  cout << "perimeterDetected: " << perimeterDetected << endl;
  cout << "firstDotDetected: " << dotDetected << endl;
  cout << "secondDotDetected: " << dotDetected2 << endl;
  cout << "dotExpectedRatio: " << dotExpectedRatio << endl;
  cout << "dotActualRatio: " << dotActualRatio << endl;

  return perimeterDetected && dotDetected && dotDetected2;
}

#endif // COMMON_H
