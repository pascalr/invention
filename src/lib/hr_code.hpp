#ifndef HR_CODE_HPP
#define HR_CODE_HPP

// An HRCode consists of a perimeter circle, two markers and text at known position.
// The inner and outer contours are both detected.
//
// With characters having a fix position, I expect the accuracy to be very high.

#include <string>

#include <iostream>
#include <cmath>
#include <stdlib.h>
#include <stdio.h>

#include <tesseract/baseapi.h>
#include <leptonica/allheaders.h>
//#include <opencv2/text/ocr.hpp>

#include <boost/log/trivial.hpp>
// trace debug info warning error fatal

#include "../lib/lib.h"
#include "../lib/opencv.h"

#include "../config/constants.h"

#include <vector>

using namespace std;
using namespace cv;

class HRCode {
  public:
    HRCode(Mat& mat, double x1, double y1, double s1) : img(mat), x(x1), y(y1), scale(s1) {
    }

    friend ostream &operator<<(std::ostream &os, const HRCode &c);

    Mat img;
    double x;
    double y;
    double scale;
};

ostream &operator<<(std::ostream &os, const HRCode &c) {
  os << "(" << c.x << ", " << c.y << ")";
  return os << "{" << c.scale << "}";
}

class HRCodeParser {
  public:
    HRCodeParser(int epsilonDia, int epsilonDist) {
    }

    int findNextCircle(int i, vector<cv::Vec4i> hierarchy, vector<bool> contourIsCircle) {
      if (i < 0) return -1;
      if (contourIsCircle[i]) {
        return i;
      } else {
        return findNextCircle(hierarchy[i][0], hierarchy, contourIsCircle);
      }
    }
    
    bool contourIsMarker(int i, int center, vector<Point2f> centers, vector<float> radius, float scale) {
    
      float expected = 26.8 * scale; // mm
      float markerDia = 4.86 * scale; // mm
    
      bool correctSize = abs(radius[i] - markerDia)/markerDia < 0.2;
      if (!correctSize) return false;
    
      float distFromCenter = sqrt(pow(centers[i].y - centers[center].y, 2)+pow(centers[i].x - centers[center].x, 2));
      BOOST_LOG_TRIVIAL(debug) << "expectedMarkerDistance: " << expected;
      BOOST_LOG_TRIVIAL(debug) << "actualMarkerDistance: " << distFromCenter;
      return abs(distFromCenter - expected)/expected < 0.2;
    }
    
    int findNextMarker(int i, vector<cv::Vec4i> hierarchy, vector<bool> contourIsCircle, vector<Point2f> centers, vector<float> radius, int center, float scale) {
      if (i < 0) return -1;
      if (contourIsCircle[i] && contourIsMarker(i,center, centers, radius, scale)) {
        return i;
      } else {
        return findNextMarker(hierarchy[i][0], hierarchy, contourIsCircle, centers, radius, center, scale);
      }
    }
    
    void findHRCodes(Mat& src, vector<HRCode> &detectedCodes, int thresh) {
      Mat src_gray;
      cvtColor( src, src_gray, COLOR_BGR2GRAY );
      blur( src_gray, src_gray, Size(3,3) ); // Remove noise

      Mat canny_output;
      Canny( src_gray, canny_output, thresh, thresh*2 );
    
      RNG rng(12345);
      vector<vector<Point>> contours;
      vector<cv::Vec4i> hierarchy;
      findContours( canny_output, contours, hierarchy, RETR_TREE, CHAIN_APPROX_SIMPLE );
      vector<Point2f> centers( contours.size() );
      vector<float> radius( contours.size() );
      vector<bool> contourIsCircle( contours.size(), false );
    
      Mat drawing = Mat::zeros( canny_output.size(), CV_8UC3 );
    
      for( size_t i = 0; i < contours.size(); i++ )
      {
          minEnclosingCircle( contours[i], centers[i], radius[i] );
          Scalar color = Scalar( rng.uniform(0, 256), rng.uniform(0,256), rng.uniform(0,256) );
          if (isBigCircle(contours[i], centers[i], radius[i], 0.2, 6)) {
            contourIsCircle[i] = true;
            circle( drawing, centers[i], (int)radius[i], color, 2 );
          }
      }

      // OPTIMIZE: Follow the three instead of going through all the leaves    
      for( size_t i = 0; i < hierarchy.size(); i++ ) {
    
        if (!contourIsCircle[i]) continue;
        
        BOOST_LOG_TRIVIAL(debug) << "Checking circle " << i;
    
        int child = hierarchy[i][2];
        if (child < 0) continue;
        
        BOOST_LOG_TRIVIAL(debug) << "Child found.";
       
        float scale = radius[i] / 36.0; // mm
        float insideDiameter = 30 * scale; // mm

        bool correctSize = abs(radius[child] - insideDiameter)/insideDiameter < 0.2;
        if (!correctSize) continue;
        
        BOOST_LOG_TRIVIAL(debug) << "Child correct size.";
    
        int firstMarker = findNextMarker(hierarchy[child][2], hierarchy, contourIsCircle, centers, radius, child, scale);
        if (firstMarker < 0) continue;
        
        BOOST_LOG_TRIVIAL(debug) << "First inner circle found.";
    
        int secondMarker = findNextMarker(hierarchy[firstMarker][0], hierarchy, contourIsCircle, centers, radius, child, scale);
        if (secondMarker < 0) continue;
        
        BOOST_LOG_TRIVIAL(debug) << "Second inner circle found.";
    
        int thirdInnerCircle = findNextMarker(hierarchy[secondMarker][0], hierarchy, contourIsCircle, centers, radius, child, scale);
        if (thirdInnerCircle >= 0) continue; // No third expected... maybe do better than that latter to remove false positives..
        
        BOOST_LOG_TRIVIAL(debug) << "Detected HRCode!!!";
        // Calculate angle
        double rise = centers[secondMarker].y - centers[firstMarker].y;
        double run = centers[secondMarker].x - centers[firstMarker].x;
        double angle_degrees;
        if (run == 0) { // edge case both circles are vertically aligned
          angle_degrees = centers[secondMarker].x > centers[i].x ? 90.0 : -90.0; // TODO: Test this is the correct way... pure guess right now
        } else {
          angle_degrees = atan2(rise, run)*180.0 / CV_PI;
    
          double avg_x = (centers[secondMarker].x + centers[firstMarker].x)/2;
          if (avg_x > centers[i].x) { // FIXME: I don't think this is 100% accurate...
            angle_degrees += 180.0;
          }
        }
    
        BOOST_LOG_TRIVIAL(debug) << "angle_degrees: "  << angle_degrees;
     
        circle( drawing, centers[i], 4, Scalar(0,0,255), FILLED );
    
        Mat rotatedHRCode;
        Mat detectedHRCode(src_gray, Rect(centers[i].x-radius[i], centers[i].y-radius[i], radius[i]*2, radius[i]*2));
        Mat rotationMatrix = cv::getRotationMatrix2D(Point2f(radius[i],radius[i]), angle_degrees, 1.0);
        warpAffine(detectedHRCode, rotatedHRCode, rotationMatrix, detectedHRCode.size());
  
        double pixelsPerMm = radius[i]*2 / HR_CODE_WIDTH;
        HRCode codePos(rotatedHRCode, centers[i].x, centers[i].y, pixelsPerMm); 
        detectedCodes.push_back(codePos);
      }
      //imshow( "Contours", drawing );
    }

};

#endif
