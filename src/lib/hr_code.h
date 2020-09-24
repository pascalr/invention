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

//#include <opencv2/text/ocr.hpp>

#include <boost/log/trivial.hpp>
// trace debug info warning error fatal

#include "../lib/lib.h"
#include "../lib/opencv.h"

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

class HRCodeParser {
  public:
    HRCodeParser(int epsilonDia, int epsilonDist) {
    }

    int findNextCircle(int i, vector<cv::Vec4i> hierarchy, vector<bool> contourIsCircle);
    
    bool contourIsMarker(int i, int center, vector<Point2f> centers, vector<float> radius, float scale);
    
    int findNextMarker(int i, vector<cv::Vec4i> hierarchy, vector<bool> contourIsCircle, vector<Point2f> centers, vector<float> radius, int center, float scale);
    
    void findHRCodes(Mat& src, vector<HRCode> &detectedCodes, int thresh);

};

#endif
