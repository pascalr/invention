#ifndef MY_LIB_OPENCV_H
#define MY_LIB_OPENCV_H

#include <opencv2/core/mat.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/core/types.hpp>
#include <exception>

class InitVideoException : public std::exception {};

bool isCircle(std::vector<cv::Point> contours, cv::Point2f center, float radius, float epsilon);

bool isBigCircle(std::vector<cv::Point> contours, cv::Point2f center, float radius, float epsilon, float minRadius);

bool initVideo(cv::VideoCapture& cap);

int captureVideoImage(cv::Mat& frame);

int captureJpeg(std::vector<uchar> &encodeBuf);

#endif
