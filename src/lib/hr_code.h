#ifndef HR_CODE_HPP
#define HR_CODE_HPP

// An HRCode consists of a perimeter circle, two markers and text at known position.
// The inner and outer contours are both detected.
//
// With characters having a fix position, I expect the accuracy to be very high.

#include <iostream>

#include "../lib/lib.h"

#include <opencv2/core/mat.hpp>

#include "../utils/constants.h"
#include "hrcode_constants.h"

#include <vector>

class HRCode {
  public:
    HRCode(cv::Mat mat, std::string imgFilename, double x1, double y1, double s1) : img(mat), imgFilename(imgFilename), x(x1), y(y1), scale(s1) {
    }
    HRCode(const HRCode& code) : img(code.img.clone()), imgFilename(code.imgFilename), x(code.x), y(code.y), scale(code.scale) {
    }

    friend std::ostream &operator<<(std::ostream &os, const HRCode &c);

    cv::Mat img;
    std::string imgFilename;
    double x;
    double y;
    double scale;
};

void extractChars(std::vector<cv::Mat>& lines, cv::Mat& src);
void extractLines(std::vector<cv::Mat>& lines, cv::Mat& src);
void parseText(std::vector<std::string>& parsedLines, cv::Mat mat);

std::ostream &operator<<(std::ostream &os, const HRCode &c);

void findHRCodes(cv::Mat& src, std::vector<HRCode> &detectedCodes, int thresh);

#endif
