#ifndef MY_LIB_OPENCV_H
#define MY_LIB_OPENCV_H

#include <opencv2/core/mat.hpp>
//#include <opencv2/videoio.hpp>
//#include <opencv2/core/types.hpp>
#include <exception>

class InitVideoException : public std::exception {};

//bool initVideo(cv::VideoCapture& cap);

int captureVideoImage(cv::Mat& frame);

int captureJpeg(std::vector<uchar> &encodeBuf);

#endif
