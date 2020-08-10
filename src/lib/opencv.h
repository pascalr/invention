#ifndef MY_LIB_OPENCV_H
#define MY_LIB_OPENCV_H

#include <opencv2/opencv.hpp>
#include <opencv2/core.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/highgui.hpp>

using namespace std;
using namespace cv;

bool isCircle(vector<Point> contours, Point2f center, float radius, float epsilon);

bool isBigCircle(vector<Point> contours, Point2f center, float radius, float epsilon, float minRadius);

bool initVideo(VideoCapture& cap);

int captureVideoImage(Mat& frame);

#endif
