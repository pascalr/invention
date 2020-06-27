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

#include <boost/log/trivial.hpp>
// trace debug info warning error fatal


#include "common.h"

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
      BOOST_LOG_TRIVIAL(error) << "expectedMarkerDistance: " << expected;
      BOOST_LOG_TRIVIAL(error) << "actualMarkerDistance: " << distFromCenter;
      return abs(distFromCenter - expected)/expected < 0.2;
    }
    
    // TODO: Put all these function is a class: HRCodeParser, so they all have access to the variables instead
    // of passing a super long quatity of variables;
    
    int findNextMarker(int i, vector<cv::Vec4i> hierarchy, vector<bool> contourIsCircle, vector<Point2f> centers, vector<float> radius, int center, float scale) {
      if (i < 0) return -1;
      if (contourIsCircle[i] && contourIsMarker(i,center, centers, radius, scale)) {
        return i;
      } else {
        return findNextMarker(hierarchy[i][0], hierarchy, contourIsCircle, centers, radius, center, scale);
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
    
        detectedCodes.push_back(rotatedHRCode);
      }
      imshow( "Contours", drawing );
    }

  protected:
};



class HRCode {
  public:
    HRCode() {}

    void setJarId(string s) {
      trim(s);
      if (s.length() != 3) {
        BOOST_LOG_TRIVIAL(debug) << "Error jar id length was " << s.length() << " expected 3.";
      }
      char* p;
      m_jar_id = strtol(s.c_str(), &p, 10);
      if (*p == 0) {
        m_is_valid = true;
      } else {
        logError("Error jar id bad conversion to number.");
      }
    }

    void setWeight(string s) {
      trim(s);
      if (s.length() != 7) {
        BOOST_LOG_TRIVIAL(debug) << "Error weight length was " << s.length() << " expected 7.";
      }
      char* p;
      m_weight = strtof(s.substr(0,5).c_str(), &p);
      if (*p == 0) {
        m_is_valid = true;
      } else {
        logError("Error weight bad conversion to number.");
      }
    }

    // Maybe latter make sure that the content id matches the name
    // but for now it doesn't.
    void setName(string s) {
      trim(s);
      m_name = s;
    }

    void setContentId(string s) {
      trim(s);
      if (s.length() != 4 && s.length() != 3) {
        BOOST_LOG_TRIVIAL(debug) << "Error content id length was " << s.length() << " expected 3 or 4.";
      }
      char* p;
      m_content_id = strtol(s.c_str(), &p, 10);
      if (*p == 0) {
        m_is_valid = true;
      } else {
        logError("Error contentId conversion to number.");
      }
    }

    friend ostream &operator<<(std::ostream &os, const HRCode &m);
    
    bool isValid() {
      return m_is_valid;
    }

  private:

    bool logError(const char* msg) {
      BOOST_LOG_TRIVIAL(info) << msg;
      return false;
    }

    bool m_is_valid = false;
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
  char config_name[] = "chars";
  char* config_ptr = config_name;
  //char config[][10] = {"chars"};
	//ocr->Init("tessdata", "eng", tesseract::OEM_LSTM_ONLY, config, 1, nullptr, nullptr, false);
	ocr->Init("tessdata", "eng", tesseract::OEM_LSTM_ONLY, &config_ptr, 1, nullptr, nullptr, false);
  ocr->SetVariable("user_defined_dpi", "300"); // FIXME: Is it
  //ocr->SetVariable("user_words_suffix", "eng.user-words"); Does this work? Rebuild and retrain my own dictionnary I think with only words that can be content.
  // But that means everytime a user adds a new product, it must retrain et rebuild everything??? Maybe, if it's fast to do...
	ocr->SetPageSegMode(tesseract::PSM_SINGLE_LINE);
  // ocr->SetPageSegMode(tesseract::PSM_SINGLE_WORD);

	ocr->SetImage(im.data, im.cols, im.rows, 3, im.step);
	string outText = string(ocr->GetUTF8Text());
  trim(outText);

	BOOST_LOG_TRIVIAL(debug) << outText;
  ocr->End();

  return outText;
}



HRCode parseHRCode(Mat& mat) {
  BOOST_LOG_TRIVIAL(debug) << "Mat cols: " << mat.cols;
  double scale = mat.cols/110.0;
  BOOST_LOG_TRIVIAL(debug) << "scale: " << scale;
  double topOffset = 10.0 * scale;
  double charWidth = 12 * scale;
  double charHeight = 26 * scale;
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
    Mat lineMat(mat, lineRect);
    imshow(string("line")+to_string(i),lineMat);
    rawHRCode[i] = parseLineTesseract(lineMat);
  }

  HRCode code;
  code.setJarId(rawHRCode[0]);
  code.setWeight(rawHRCode[1]);
  code.setName(rawHRCode[2]);
  code.setContentId(rawHRCode[3]);
  if (code.isValid()) {
    BOOST_LOG_TRIVIAL(info) << "Detected code: " << code;
  }

  return code;
}

vector<HRCode> detectHRCodes(Mat& src) {
  Mat src_gray;
  int thresh = 100;
  const char* source_window = "Source";
  const int max_thresh = 255;

  cvtColor( src, src_gray, COLOR_BGR2GRAY );
  blur( src_gray, src_gray, Size(3,3) ); // Remove noise

  /*Mat dilate_output;
  Mat element = getStructuringElement( MORPH_RECT, Size(3,3), Point(1, 1) );
  dilate( src_gray, dilate_output, element );*/

  HRCodeParser parser(0.2, 0.2);
  vector<Mat> hr_codes;
  parser.findHRCode(src_gray, hr_codes, thresh);

  vector<HRCode> codes(hr_codes.size());

  for( size_t i = 0; i < hr_codes.size(); i++ ) {
    Mat m = hr_codes[i];

    cvtColor( m, m, COLOR_GRAY2BGR );
    codes[i] = parseHRCode(m);

    string title = string("detectedHRCode") + to_string(i) + ".png";
    imshow(title,m);
    imwrite(title,m);
  }

  return codes;
}

#endif
