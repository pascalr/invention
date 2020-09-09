#include "hr_code.h"

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
#include "../utils/io_common.h"

#include "../core/tess_parser.h" // tmp

#include "../utils/constants.h"

#include <opencv2/opencv.hpp>
#include <opencv2/core.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/highgui.hpp>

#include <vector>

using namespace std;
using namespace cv;

ostream &operator<<(std::ostream &os, const HRCode &c) {
  os << "(" << c.x << ", " << c.y << ")";
  return os << "{" << c.scale << "}";
}

// Extract chars this way does not work, because sometimes there is an even nb of chars, sometimes there is an odd.
void extractLines(vector<Mat>& lines, Mat& src) {

  double scale = src.cols/110.0;
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
    double x = nbChar/-2.0*charWidth + src.cols/2.0;
    Rect lineRect = Rect(x, y, nbChar*charWidth, charHeight);
    //rectangle(src, lineRect, Scalar(0,255,0), 2, LINE_8);
    Mat lineMat(src, lineRect);
    lines.push_back(lineMat);
  }
}

class ImageProcess {
  public:
    virtual void process(Mat& src, Mat& dest) = 0;
};

class Scorer {
  public:
    double score(string expected, string actual) {
      // +5 per good character, -1 per bad character
      return 1.0;
    }
};

// Automatically detect the best combination of process to get the best accuracy.
class Optimiser {
  public:
    vector<ImageProcess> processes;
    Scorer scorer;
};

class DilateProcess : public ImageProcess {
  public:

    void process(Mat& src, Mat& dest) {
      int dilateSize = 2; // TODO: Make this a param
      Mat kernel = getStructuringElement( MORPH_RECT, Size( 2*dilateSize + 1, 2*dilateSize+1 ), Point( dilateSize, dilateSize ) );
      dilate(src, dest, kernel );
    }

};

class AdaptiveThresholdProcess : public ImageProcess {
  public:

    void process(Mat& src, Mat& dest) {
      adaptiveThreshold(src, dest, 255, ADAPTIVE_THRESH_MEAN_C, THRESH_BINARY, 3, 5 );
    }

};

// Adjusts contrast. Darkens too white images and whitens too dark images.
class BrightnessProcess : public ImageProcess {
  public:

    void process(Mat& src, Mat& dest) {
      double averageBrightness = 0;
      for(int i=0; i<src.rows; i++) {
        for(int j=0; j<src.cols; j++) {
          averageBrightness += src.at<uchar>(i,j);
        }
      }
      averageBrightness /= src.rows*src.cols;
      std::cout << "Avg brightness: " << averageBrightness << std::endl;

      double requiredBrightness = 255.0 / 2.0;
      double factor = requiredBrightness / averageBrightness;

      dest = src*factor;
    }

};

class ErodeProcess : public ImageProcess  {
  public:

    void process(Mat& src, Mat& dest) {
      int erosionSize = 2; // TODO: Make this a param
      Mat kernel = getStructuringElement( MORPH_RECT, Size( 2*erosionSize + 1, 2*erosionSize+1 ), Point( erosionSize, erosionSize ) );
      erode(src, dest, kernel );
    }

};

class ThresholdBinaryProcess : public ImageProcess  {
  public:

    void process(Mat& src, Mat& dest) {
      threshold(src, dest, 255/2.0, 255, THRESH_BINARY);
    }

};

// Make everything below the threshold black
class ThresholdBlackProcess : public ImageProcess  {
  public:

    void process(Mat& src, Mat& dest) {
      threshold(src, dest, 80, 255, THRESH_TOZERO);
    }

};

// Make everything above (255-threshold) white
class ThresholdWhiteProcess : public ImageProcess  {
  public:

    void process(Mat& src, Mat& dest) {
      threshold(src, dest, 80, 255, THRESH_TOZERO);
    }

};

class ContrastProcess : public ImageProcess  {
  public:
    void process(Mat& src, Mat& dest) {
      double contrast = 1.1;
      dest = src * contrast;
    }
};


class BlurProcess : public ImageProcess  {
  public:
    void process(Mat& src, Mat& dest) {
      blur(src, dest, Size(3,3)); // Remove noise
    }
};

class CannyProcess : public ImageProcess  {
  public:
    void process(Mat& src, Mat& dest) {
      int thresh = 20;
      Canny(src, dest, thresh, thresh*2 );
    }
};

// CannyProcess should be run just before
class DrawContoursProcess : public ImageProcess  {
  public:
    void process(Mat& src, Mat& dest) {
      RNG rng(12345);
      vector<vector<Point>> contours;
      vector<cv::Vec4i> hierarchy;
      findContours( src, contours, hierarchy, RETR_TREE, CHAIN_APPROX_SIMPLE );

      int idx = 0;
      for(; idx >= 0; idx = hierarchy[idx][0] )
      {
          Scalar color( rand()&255, rand()&255, rand()&255 );
          drawContours( dest, contours, idx, color, FILLED, 8, hierarchy );
      }
    }
};

// Given a point, I want everything above to get brighter, and everything below to get darker, and more the farther it gets.
/*class MyContrastProcess {
  public:
    void process(Mat& src, Mat& dest) {
      double midPoint = 0.5;
      for( int y = 0; y < src.rows; y++ ) {
          for( int x = 0; x < src.cols; x++ ) {
              for( int c = 0; c < src.channels(); c++ ) {
                  dest.at<Vec3b>(y,x)[c] =
                    saturate_cast<uchar>( (image.at<Vec3b>(y,x)[c] / 255.0) );
                    //saturate_cast<uchar>( alpha*image.at<Vec3b>(y,x)[c] + beta );
              }
          }
      }
    }
};
*/

// TODO Contours process

// Find characters with contours does not work because they are too close together
void parseText(vector<string>& parsedLines, Mat gray) {
  
  resize(gray, gray, Size(gray.cols*2, gray.rows*2), 0, 0, INTER_AREA);

 
  Mat dst = gray.clone(); 

  vector<shared_ptr<ImageProcess>> processes;
  processes.push_back(make_shared<BlurProcess>());
  processes.push_back(make_shared<BrightnessProcess>());
  //processes.push_back(make_shared<ErodeProcess>());
  //processes.push_back(make_shared<CannyProcess>());
  //processes.push_back(make_shared<DrawContoursProcess>());
  //processes.push_back(make_shared<AdaptiveThresholdProcess>());
  //processes.push_back(make_shared<DilateProcess>());
  //processes.push_back(make_shared<DilateProcess>());
  //processes.push_back(make_shared<ThresholdBinaryProcess>());
  
  Mat sideBySide = gray.clone();

  for (shared_ptr<ImageProcess>& process : processes) {
    process->process(dst, dst);
    hconcat(sideBySide, dst, sideBySide);
  }


  vector<Mat> lines;
  extractLines(lines, dst);

  TessParser parser;
  parsedLines.push_back(parser.parseDigitLine(lines[0]));
  parsedLines.push_back(parser.parseLine(lines[1]));
  parsedLines.push_back(parser.parseLine(lines[2]));
  parsedLines.push_back(parser.parseDigitLine(lines[3]));
  for (const string& line : parsedLines) {
    cout << "Detected code: " << line << endl;
  }

  //imshow("show_side_by_side",sideBySide);
  //waitKey(0);

}

int HRCodeParser::findNextCircle(int i, vector<cv::Vec4i> hierarchy, vector<bool> contourIsCircle) {
  if (i < 0) return -1;
  if (contourIsCircle[i]) {
    return i;
  } else {
    return findNextCircle(hierarchy[i][0], hierarchy, contourIsCircle);
  }
}

bool HRCodeParser::contourIsMarker(int i, int center, vector<Point2f> centers, vector<float> radius, float scale) {

  float expected = 26.8 * scale; // mm
  float markerDia = 4.86 * scale; // mm

  bool correctSize = abs(radius[i] - markerDia)/markerDia < 0.2;
  if (!correctSize) return false;

  float distFromCenter = sqrt(pow(centers[i].y - centers[center].y, 2)+pow(centers[i].x - centers[center].x, 2));
  //BOOST_LOG_TRIVIAL(debug) << "expectedMarkerDistance: " << expected;
  //BOOST_LOG_TRIVIAL(debug) << "actualMarkerDistance: " << distFromCenter;
  return abs(distFromCenter - expected)/expected < 0.2;
}

int HRCodeParser::findNextMarker(int i, vector<cv::Vec4i> hierarchy, vector<bool> contourIsCircle, vector<Point2f> centers, vector<float> radius, int center, float scale) {
  if (i < 0) return -1;
  if (contourIsCircle[i] && contourIsMarker(i,center, centers, radius, scale)) {
    return i;
  } else {
    return findNextMarker(hierarchy[i][0], hierarchy, contourIsCircle, centers, radius, center, scale);
  }
}

void HRCodeParser::findHRCodes(Mat& src, vector<HRCode> &detectedCodes, int thresh) {
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
    
    // TODO: Return an error message with the reason why it did not detect instead of logging stuff like this.
    //BOOST_LOG_TRIVIAL(debug) << "Checking circle " << i;

    int child = hierarchy[i][2];
    if (child < 0) continue;
    
    //BOOST_LOG_TRIVIAL(debug) << "Child found.";
   
    float scale = radius[i] / 36.0; // mm
    float insideDiameter = 30 * scale; // mm

    bool correctSize = abs(radius[child] - insideDiameter)/insideDiameter < 0.2;
    if (!correctSize) continue;
    
    //cout << "HRCode Perimeter found." << endl;

    int firstMarker = findNextMarker(hierarchy[child][2], hierarchy, contourIsCircle, centers, radius, child, scale);
    if (firstMarker < 0) continue;
    
    //cout << "HRCode first marker found." << endl;

    int secondMarker = findNextMarker(hierarchy[firstMarker][0], hierarchy, contourIsCircle, centers, radius, child, scale);
    if (secondMarker < 0) continue;
    
    //cout << "HRCode second marker found." << endl;

    int thirdInnerCircle = findNextMarker(hierarchy[secondMarker][0], hierarchy, contourIsCircle, centers, radius, child, scale);
    if (thirdInnerCircle >= 0) continue; // No third expected... maybe do better than that latter to remove false positives..
    
    //cout << "Detected HRCode!" << endl;
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

    //BOOST_LOG_TRIVIAL(debug) << "angle_degrees: "  << angle_degrees;
 
    circle( drawing, centers[i], 4, Scalar(0,0,255), FILLED );

    Mat rotatedHRCode;
    Mat detectedHRCode(src_gray, Rect(centers[i].x-radius[i], centers[i].y-radius[i], radius[i]*2, radius[i]*2));
    Mat rotationMatrix = cv::getRotationMatrix2D(Point2f(radius[i],radius[i]), angle_degrees, 1.0);
    warpAffine(detectedHRCode, rotatedHRCode, rotationMatrix, detectedHRCode.size());

    double pixelsPerMm = radius[i]*2 / HR_CODE_WIDTH;
    string filename = nextFilename(DETECTED_CODES_BASE_PATH, "detected_code", ".jpg");
    string imgFilename = DETECTED_CODES_BASE_PATH + filename;
    imwrite(imgFilename, rotatedHRCode);
    HRCode codePos(rotatedHRCode, filename, centers[i].x, centers[i].y, pixelsPerMm); 
    detectedCodes.push_back(codePos);
  }
  //imshow( "Contours", drawing );
}
