#ifndef JAR_PARSER_H
#define JAR_PARSER_H

#include <opencv2/core/mat.hpp>

std::string parseLine(const cv::Mat& im);
std::string parseDigitLine(const cv::Mat& im);
std::string parseDefault(const cv::Mat& im);

#endif
