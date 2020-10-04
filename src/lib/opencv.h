#ifndef MY_LIB_OPENCV_H
#define MY_LIB_OPENCV_H

#include <opencv2/core/mat.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/core/types.hpp>
#include <exception>

using namespace std;
using namespace cv;

class InitVideoException : public exception {};

bool isCircle(vector<Point> contours, Point2f center, float radius, float epsilon);

bool isBigCircle(vector<Point> contours, Point2f center, float radius, float epsilon, float minRadius);

bool initVideo(VideoCapture& cap);

int captureVideoImage(Mat& frame);

int captureJpeg(vector<uchar> &encodeBuf);

#endif
