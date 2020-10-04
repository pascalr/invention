#ifndef HR_CODE_HPP
#define HR_CODE_HPP

// An HRCode consists of a perimeter circle, two markers and text at known position.
// The inner and outer contours are both detected.
//
// With characters having a fix position, I expect the accuracy to be very high.

#include <iostream>
#include <cmath>
#include <stdlib.h>
#include <stdio.h>

#include "../lib/lib.h"

#include <opencv2/core/mat.hpp>

#include "../utils/constants.h"
#include "hrcode_constants.h"

#include <vector>

using namespace std;
using namespace cv;

class HRCode {
  public:
    HRCode(Mat mat, std::string imgFilename, double x1, double y1, double s1) : img(mat), imgFilename(imgFilename), x(x1), y(y1), scale(s1) {
    }
    HRCode(const HRCode& code) : img(code.img.clone()), imgFilename(code.imgFilename), x(code.x), y(code.y), scale(code.scale) {
    }

    friend ostream &operator<<(std::ostream &os, const HRCode &c);

    Mat img;
    string imgFilename;
    double x;
    double y;
    double scale;
};

void extractChars(vector<Mat>& lines, Mat& src);
void extractLines(vector<Mat>& lines, Mat& src);
void parseText(vector<string>& parsedLines, Mat mat);

ostream &operator<<(std::ostream &os, const HRCode &c);

void findHRCodes(Mat& src, vector<HRCode> &detectedCodes, int thresh);

#endif
