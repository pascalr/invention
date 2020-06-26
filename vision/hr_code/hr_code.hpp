#ifndef HR_CODE_HPP
#define HR_CODE_HPP

// An HRCode consists of a perimeter circle, two markers and text at known position.
// The inner and outer contours are both detected.
//
// With characters having a fix position, I expect the accuracy to be very high.

#include <opencv2/opencv.hpp>
#include "opencv2/imgproc.hpp"
#include "opencv2/highgui.hpp"
#include <iostream>
#include <cmath>
#include <stdlib.h>
#include <stdio.h>

#include <tesseract/baseapi.h>
#include <leptonica/allheaders.h>
//#include <opencv2/text/ocr.hpp>

#include "../common.h"

int findNextCircle(int i, vector<cv::Vec4i> hierarchy, vector<bool> contourIsCircle) {
  if (i < 0) return -1;
  if (contourIsCircle[i]) {
    return i;
  } else {
    return findNextCircle(hierarchy[i][0], hierarchy, contourIsCircle);
  }
}

void findHRCode(Mat src_gray, vector<Mat> &detectedCodes, int thresh) {
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

  for( size_t i = 0; i < hierarchy.size(); i++ ) {

    if (!contourIsCircle[i]) continue;
    
    cout << "Checking circle " << i << endl;

    int child = hierarchy[i][2];
    if (child < 0) continue;
    
    cout << "Child found." << endl;

    int firstInnerCircle = findNextCircle(hierarchy[child][2], hierarchy, contourIsCircle);
    if (firstInnerCircle < 0) continue;
    
    cout << "First inner circle found." << endl;

    int secondInnerCircle = findNextCircle(hierarchy[firstInnerCircle][0], hierarchy, contourIsCircle);
    if (secondInnerCircle < 0) continue;
    
    cout << "Second inner circle found." << endl;

    int thirdInnerCircle = findNextCircle(hierarchy[secondInnerCircle][0], hierarchy, contourIsCircle);
    if (thirdInnerCircle >= 0) continue; // No third expected... maybe do better than that latter to remove false positives..
    
    cout << "No third inner circle found." << endl;
      
    cout << "Possible candidate!!!" << endl;

    if (isValidHRCode(i, child, firstInnerCircle, secondInnerCircle, centers, radius)) {
      cout << "Detected HRCode!!!" << endl;
      // Calculate angle
      double rise = centers[secondInnerCircle].y - centers[firstInnerCircle].y;
      double run = centers[secondInnerCircle].x - centers[firstInnerCircle].x;
      double angle_degrees;
      if (run == 0) { // edge case both circles are vertically aligned
        angle_degrees = centers[secondInnerCircle].x > centers[i].x ? 90.0 : -90.0; // TODO: Test this is the correct way... pure guess right now
      } else {
        angle_degrees = atan2(rise, run)*180.0 / CV_PI;

        double avg_x = (centers[secondInnerCircle].x + centers[firstInnerCircle].x)/2;
        if (avg_x > centers[i].x) { // FIXME: I don't think this is 100% accurate...
          angle_degrees += 180.0;
        }
      }

      cout << "angle_degrees: "  << angle_degrees << endl;
 
      circle( drawing, centers[i], 4, Scalar(0,0,255), FILLED );

      Mat rotatedHRCode;
      Mat detectedHRCode(src_gray, Rect(centers[i].x-radius[i], centers[i].y-radius[i], radius[i]*2, radius[i]*2));
      Mat rotationMatrix = cv::getRotationMatrix2D(Point2f(radius[i],radius[i]), angle_degrees, 1.0);
      warpAffine(detectedHRCode, rotatedHRCode, rotationMatrix, detectedHRCode.size());

      detectedCodes.push_back(rotatedHRCode);
    }
  }
  imshow( "Contours", drawing );
}

class HRCode {
  public:
    HRCode() {}

    bool setJarId(string s) {
      trim(s);
      if (s.length() != 3) {
        cout << "Error jar id length was " << s.length() << " expected 3." << endl;
        return false;
      }
      char* p;
      m_jar_id = strtol(s.c_str(), &p, 10);
      return *p == 0 || logError("Error jar id bad conversion to number.");
    }

    bool setWeight(string s) {
      trim(s);
      if (s.length() != 7) {
        cout << "Error weight length was " << s.length() << " expected 7." << endl;
        return false;
      }
      char* p;
      m_weight = strtof(s.substr(0,5).c_str(), &p);
      return *p == 0 || logError("Error weight bad conversion to number.");
    }

    // Maybe latter make sure that the content id matches the name
    // but for now it doesn't.
    bool setName(string s) {
      trim(s);
      m_name = s;
      return true;
    }

    bool setContentId(string s) {
      trim(s);
      if (s.length() != 4 && s.length() != 3) {
        cout << "Error content id length was " << s.length() << " expected 3 or 4." << endl;
        return false;
      }
      char* p;
      m_content_id = strtol(s.c_str(), &p, 10);
      return *p == 0 || logError("Error content id bad conversion to number.");
    }

    friend ostream &operator<<(std::ostream &os, const HRCode &m);
    

  private:

    bool logError(const char* msg) {
      cout << msg;
      return false;
    }

    int m_jar_id = -1;
    float m_weight = 0;
    string m_name;
    int m_content_id;
};

ostream &operator<<(std::ostream &os, const HRCode &m) {
  os << "#" << m.m_jar_id << "[" << m.m_weight << " kg]: ";
  return os << m.m_name << " (" << m.m_content_id << ")";
}

string parseLineTesseract(Mat& im) {
	tesseract::TessBaseAPI *ocr = new tesseract::TessBaseAPI();
	ocr->Init("tessdata", "eng", tesseract::OEM_LSTM_ONLY);
	ocr->SetPageSegMode(tesseract::PSM_SINGLE_LINE);
  // ocr->SetPageSegMode(tesseract::PSM_SINGLE_WORD);

	ocr->SetImage(im.data, im.cols, im.rows, 3, im.step);
	string outText = string(ocr->GetUTF8Text());

	cout << outText << endl;
  ocr->End();

  return outText;
}

void parseHRCode(Mat& mat) {
  cout << "Mat cols: " << mat.cols << endl;
  double scale = mat.cols/110.0;
  cout << "scale: " << scale << endl;
  double topOffset = 8.0 * scale;
  double charWidth = 11 * scale;
  double charHeight = 24 * scale;
  double lineInterspace = 24 * scale;

  int nbLines = 4;
  int pattern[nbLines] = {3,7,8,4}; // number of char per line
  string rawHRCode[nbLines];

  // Get the sub-matrices (minors) for every character.
  for (int i = 0; i < nbLines; i++) {
    int nbChar = pattern[i];
    double y = i*lineInterspace + topOffset;
    for (int j = 0; j < nbChar; j++) {
      double x = (0.0+j-1.0*nbChar/2.0)*charWidth + mat.cols/2;
      Rect r = Rect(x, y, charWidth, charHeight);
      //rectangle(mat, r, Scalar(0,0,255), 1, LINE_8);
      //Mat charMat(mat, Rect(x, y, charWidth, charHeight));
    }
    Rect lineRect = Rect(nbChar/-2.0*charWidth + mat.cols/2, y, nbChar*charWidth, charHeight);
    //rectangle(mat, lineRect, Scalar(0,255,0), 1, LINE_8);
    string title = string("line_") + to_string(i) + ".png";
    Mat lineMat(mat, lineRect);
    imwrite(title,lineMat);
    rawHRCode[i] = parseLineTesseract(lineMat);
  }

  HRCode code;
  if (code.setJarId(rawHRCode[0]) && code.setWeight(rawHRCode[1])
      && code.setName(rawHRCode[2]) && code.setContentId(rawHRCode[3])) {
    cout << "Detected code: " << code << endl;
  }
}

#endif
