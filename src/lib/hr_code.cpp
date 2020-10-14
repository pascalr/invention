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

// trace debug info warning error fatal

#include "../lib/lib.h"
#include "../lib/opencv.h"
#include "../utils/io_common.h"

#include "../core/tess_parser.h"

#include "../utils/constants.h"

#include <opencv2/opencv.hpp>
#include <opencv2/core.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/core/types.hpp>

#include <vector>

#include "near_duplicates.h"

using namespace std;
using namespace cv;

bool isCircle(vector<Point> contours, Point2f center, float radius, float epsilon) {
  // epsilon is a percentage of the radius
  // no more than 10% variation
  float maxVariation = radius * epsilon;
  bool isACircle = true;
  for( size_t i = 0; i < contours.size(); i++ ) {
    double norm = sqrt(pow(contours[i].x - center.x, 2)+pow(contours[i].y - center.y, 2));
    isACircle = isACircle && abs(norm-radius) < maxVariation;
  }
  return isACircle;
}

bool isBigCircle(vector<Point> contours, Point2f center, float radius, float epsilon, float minRadius) {
  return radius > minRadius && isCircle(contours, center, radius, epsilon);
}


ostream &operator<<(std::ostream &os, const HRCode &c) {
  os << "(" << c.x << ", " << c.y << ")";
  return os << "{" << c.scale << "}";
}

void extractLine(cv::Mat& mat, int lineNb, const cv::Mat& src, double lineWidthMm) {
  
  double scale = src.cols/HRCODE_OUTER_DIA; // px per mm

  double lineInterspace = HRCODE_LINE_INTERSPACE * scale; // px
  double lineWidth = lineWidthMm * 1.11 * scale; // px
  double charHeight = HRCODE_CHAR_HEIGHT * 1.75 * scale; // px
 
  double firstLineY = HRCODE_MARKERS_DIST_Y_FROM_MIDDLE * scale; // px
  double x = src.cols/2.0 - lineWidth/2.0; // px
  double y = src.rows/2.0 - firstLineY - charHeight/2.0 + lineNb*lineInterspace; // px

  Rect lineRect = Rect(x, y, lineWidth, charHeight);
  //rectangle(src, lineRect, Scalar(0,255,0), 2, LINE_8);
  cv::Mat lineMat(src, lineRect);
  mat = lineMat.clone();
}

// Extract chars this way does not work, because sometimes there is an even nb of chars, sometimes there is an odd.
void extractLines(vector<cv::Mat>& lines, cv::Mat& src) {

  cv::Mat jarId;
  extractLine(jarId, 0, src, HRCODE_LINE_0_WIDTH);
  lines.push_back(jarId);
  //imshow("jarId", jarId);
  //waitKey(0);
    
 // SDL_RenderDrawLine(gRenderer, centerX-mmToPx(HRCODE_LINE_1_WIDTH/2.0), textY+mmToPx(HRCODE_LINE_INTERSPACE*1), centerX+mmToPx(HRCODE_LINE_1_WIDTH/2.0), textY+mmToPx(HRCODE_LINE_INTERSPACE*1));
 // SDL_RenderDrawLine(gRenderer, centerX-mmToPx(HRCODE_LINE_2_WIDTH/2.0), textY+mmToPx(HRCODE_LINE_INTERSPACE*2), centerX+mmToPx(HRCODE_LINE_2_WIDTH/2.0), textY+mmToPx(HRCODE_LINE_INTERSPACE*2));
 // SDL_RenderDrawLine(gRenderer, centerX-mmToPx(HRCODE_LINE_3_WIDTH/2.0), textY+mmToPx(HRCODE_LINE_INTERSPACE*3), centerX+mmToPx(HRCODE_LINE_3_WIDTH/2.0), textY+mmToPx(HRCODE_LINE_INTERSPACE*3));

  // Get the sub-matrices (minors) for every character.
}

class ImageProcess {
  public:
    virtual void process(cv::Mat& src, cv::Mat& dest) = 0;
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

    void process(cv::Mat& src, cv::Mat& dest) {
      int dilateSize = 2; // TODO: Make this a param
      cv::Mat kernel = getStructuringElement( MORPH_RECT, Size( 2*dilateSize + 1, 2*dilateSize+1 ), Point( dilateSize, dilateSize ) );
      dilate(src, dest, kernel );
    }

};

class AdaptiveThresholdProcess : public ImageProcess {
  public:

    void process(cv::Mat& src, cv::Mat& dest) {
      adaptiveThreshold(src, dest, 255, ADAPTIVE_THRESH_MEAN_C, THRESH_BINARY, 3, 5 );
    }

};

// Adjusts contrast. Darkens too white images and whitens too dark images.
class BrightnessProcess : public ImageProcess {
  public:

    void process(cv::Mat& src, cv::Mat& dest) {
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

    void process(cv::Mat& src, cv::Mat& dest) {
      int erosionSize = 2; // TODO: Make this a param
      cv::Mat kernel = getStructuringElement( MORPH_RECT, Size( 2*erosionSize + 1, 2*erosionSize+1 ), Point( erosionSize, erosionSize ) );
      erode(src, dest, kernel );
    }

};

class ThresholdBinaryProcess : public ImageProcess  {
  public:

    void process(cv::Mat& src, cv::Mat& dest) {
      threshold(src, dest, 255/2.0, 255, THRESH_BINARY);
    }

};

// Make everything below the threshold black
class ThresholdBlackProcess : public ImageProcess  {
  public:

    void process(cv::Mat& src, cv::Mat& dest) {
      threshold(src, dest, 80, 255, THRESH_TOZERO);
    }

};

// Make everything above (255-threshold) white
class ThresholdWhiteProcess : public ImageProcess  {
  public:

    void process(cv::Mat& src, cv::Mat& dest) {
      threshold(src, dest, 80, 255, THRESH_TOZERO);
    }

};

class ContrastProcess : public ImageProcess  {
  public:
    void process(cv::Mat& src, cv::Mat& dest) {
      double contrast = 1.1;
      dest = src * contrast;
    }
};


class BlurProcess : public ImageProcess  {
  public:
    void process(cv::Mat& src, cv::Mat& dest) {
      blur(src, dest, Size(3,3)); // Remove noise
    }
};

class CannyProcess : public ImageProcess  {
  public:
    void process(cv::Mat& src, cv::Mat& dest) {
      int thresh = 20;
      Canny(src, dest, thresh, thresh*2 );
    }
};

// CannyProcess should be run just before
class DrawContoursProcess : public ImageProcess  {
  public:
    void process(cv::Mat& src, cv::Mat& dest) {
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
void parseText(vector<string>& parsedLines, cv::Mat gray) {
  
  resize(gray, gray, Size(gray.cols*2, gray.rows*2), 0, 0, INTER_AREA);
 
  cv::Mat dst = gray.clone(); 

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
  
  cv::Mat sideBySide = gray.clone();

  for (shared_ptr<ImageProcess>& process : processes) {
    process->process(dst, dst);
    hconcat(sideBySide, dst, sideBySide);
  }

  vector<cv::Mat> lines;
  extractLines(lines, dst);

  parsedLines.push_back(parseDigitLine(lines[0]));
  //parsedLines.push_back(parser.parseLine(lines[1]));
  //parsedLines.push_back(parser.parseLine(lines[2]));
  //parsedLines.push_back(parser.parseDigitLine(lines[3]));
  for (const string& line : parsedLines) {
    cout << "Detected code: " << line << endl;
  }

  //imshow("show_side_by_side",sideBySide);
  //waitKey(0);

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

  float expectedDist = sqrt(HRCODE_MARKERS_DIST_FROM_MIDDLE_SQ); // mm
  float markerRadius = radius[i] / scale; // mm

  bool correctSize = abs(markerRadius - HRCODE_MARKER_RADIUS)/HRCODE_MARKER_RADIUS < 0.2;
  if (!correctSize) { std::cout << "Expected marker dia to be " << HRCODE_MARKER_RADIUS << ", but was: " << markerRadius << std::endl; return false; }

  float distFromCenter = sqrt(pow(centers[i].y - centers[center].y, 2)+pow(centers[i].x - centers[center].x, 2))/scale;
  //BOOST_LOG_TRIVIAL(debug) << "expectedMarkerDistance: " << expected;
  //BOOST_LOG_TRIVIAL(debug) << "actualMarkerDistance: " << distFromCenter;
  bool correctDist = abs(distFromCenter - expectedDist)/expectedDist < (0.2);
  if (!correctDist) { std::cout << "Expected marker dist to be " << expectedDist << ", but was: " << distFromCenter << std::endl; return false; }
  return true;
}

int findNextMarker(int i, vector<cv::Vec4i> hierarchy, vector<bool> contourIsCircle, vector<Point2f> centers, vector<float> radius, int center, float scale) {
  if (i < 0) return -1;
  if (contourIsCircle[i] && contourIsMarker(i,center, centers, radius, scale)) {
    return i;
  } else {
    return findNextMarker(hierarchy[i][0], hierarchy, contourIsCircle, centers, radius, center, scale);
  }
}

class CircleDetected {
  public:
    // OPTIMIZE: Use faster distanceSquared, whatever...
    double distancePx(CircleDetected c) {
      return sqrt(pow(centerX - c.centerX, 2) + pow(centerY - c.centerY, 2));
    }
    double distanceMm(CircleDetected c, double pixelsPerMm) {
      return distancePx(c) / pixelsPerMm;
    }
    double radiusMm(double pixelsPerMm) {
      return radiusPx / pixelsPerMm;
    }
    int id;
    double radiusPx;
    double centerX;
    double centerY;
};

bool hasInnerPerimeter(CircleDetected c, vector<CircleDetected> circles, double pixelsPerMm) {
  for (CircleDetected& circle : circles) {
    if (c.id == circle.id) continue;

    bool correctSize = abs(circle.radiusMm(pixelsPerMm) - HRCODE_INNER_RADIUS)/HRCODE_INNER_RADIUS < 0.2; // HARDCODED. 20% is very generous
    bool sameCenter = c.distanceMm(circle, pixelsPerMm) < 4; // HARDCODED. 4 mm is very generous, but OK for now.
    if (correctSize && sameCenter) return true;
  }
  return false;
}

vector<CircleDetected> findAllMarkers(CircleDetected c, vector<CircleDetected> circles, double pixelsPerMm) {

  float expectedDist = sqrt(HRCODE_MARKERS_DIST_FROM_MIDDLE_SQ); // mm

  vector<CircleDetected> result;

  for (CircleDetected& circle : circles) {

    bool correctSize = abs(circle.radiusMm(pixelsPerMm) - HRCODE_MARKER_RADIUS)/HRCODE_MARKER_RADIUS < 0.2; // HARDCODED. 20% is a little generous
    bool correctDistance = abs(c.distanceMm(circle, pixelsPerMm) - expectedDist)/expectedDist < 0.2; // HARDCODED. 20% is a little generous
    if (correctSize && correctDistance) {
      result.push_back(circle);
    }
  }

  return result;
}

// OK, new algorithm needed... I cannot use the hierarchies...
// Because when only an arc of a circle is detected, than it is not considered a container.
// But for me an arc is enough. The contours detector offen detects multiple edges for the same circle edge.
//
// We find all the circles.
// Then for each circle, we assume it is the outer perimeter.
// We then check if it has an inner perimeter.
// We then check if it has two and only two markers.
void findHRCodes(cv::Mat& src, vector<HRCode> &detectedCodes, int thresh) {
  cv::Mat src_gray;
  cvtColor( src, src_gray, COLOR_BGR2GRAY );
  blur( src_gray, src_gray, Size(3,3) ); // Remove noise

  cv::Mat canny_output;
  Canny( src_gray, canny_output, thresh, thresh*2 );

  RNG rng(12345);
  vector<vector<Point>> contours;
  vector<cv::Vec4i> hierarchy;
  findContours( canny_output, contours, hierarchy, RETR_TREE, CHAIN_APPROX_SIMPLE );
  vector<Point2f> centers( contours.size() );
  vector<float> radius( contours.size() );
  //vector<bool> contourIsCircle( contours.size(), false );
  vector<CircleDetected> circles;

  cv::Mat circlesDrawing = cv::Mat::zeros( canny_output.size(), CV_8UC3 ); // DEBUG ONLY
  cv::Mat contoursDrawing = cv::Mat::zeros( canny_output.size(), CV_8UC3 ); // DEBUG ONLY

  for( size_t i = 0; i < contours.size(); i++ )
  {
      minEnclosingCircle( contours[i], centers[i], radius[i] );

      Scalar color = Scalar( rng.uniform(0, 256), rng.uniform(0,256), rng.uniform(0,256) ); // DEBUG ONLY
      drawContours( contoursDrawing, contours, i, color, 3, cv::LINE_8, hierarchy ); // DEBUG ONLY

      // FIXME: Minimum circle is 6 pixels wide, is that ok?
      if (isBigCircle(contours[i], centers[i], radius[i], 0.2, 6)) {
        //contourIsCircle[i] = true;
        circle( circlesDrawing, centers[i], (int)radius[i], color, 3 ); // DEBUG ONLY

        CircleDetected c;
        c.id = i;
        c.radiusPx = radius[i];
        c.centerX = centers[i].x;
        c.centerY = centers[i].y;
        circles.push_back(c);
      }
  }

  for (CircleDetected& circle : circles) {

    // Assume the circle is the outer perimeter of the code
    float pixelsPerMm = circle.radiusPx / HRCODE_OUTER_RADIUS;
    std::cout << "Detected a circle with a radius of " << circle.radiusPx << " pixels. Pixels per mm: " << pixelsPerMm << std::endl;
    // Maybe output what it is expecting it to be? Would that help? For example: expects an inner circle of ... px.

    if (!hasInnerPerimeter(circle, circles, pixelsPerMm)) {
      std::cout << "No perimeter was found for this circle." << std::endl; continue;
    }
 
    
    // The difference in size is big between the perimeter and the markers, so it does not matter if
    // checking the perimeters in the list for nothing.
    vector<CircleDetected> markers = findAllMarkers(circle, circles, pixelsPerMm);
    if (markers.size() < 2) { std::cout << "Did not detect 2 markers. Detected: " << markers.size() << std::endl; continue; }

    CircleDetected firstMarker = markers[0];
    CircleDetected secondMarker = markers[1];

    if (markers.size() > 2) {
      std::function<double(CircleDetected,CircleDetected)> func = [pixelsPerMm](CircleDetected c1, CircleDetected c2) -> double {return c1.distanceMm(c2,pixelsPerMm);};
      vector<vector<size_t>> groups = groupNearDuplicates(markers, func, 4.0); // HARDCODED. 4mm. Very generous
      if (groups.size() > 2) { std::cout << "Detected too many markers. Expected only 2. Detected: " << groups.size() << std::endl; continue;}
      firstMarker = markers[groups[0][0]];
      secondMarker = markers[groups[1][0]];
    }

    bool correctDistance = abs(firstMarker.distanceMm(secondMarker, pixelsPerMm) - HRCODE_MARKERS_INTERSPACE)/HRCODE_MARKERS_INTERSPACE < 0.2; // HARDCODED. 20% is a little generous
    if (!correctDistance) {std::cout << "The interspace between the two markers was not at the correct distance." << std::endl; continue;}

    cout << "Detected HRCode!" << endl;

    // Calculate angle
    double rise = firstMarker.centerY - secondMarker.centerY;
    double run = secondMarker.centerX - firstMarker.centerX;
    double angle_degrees;
    if (run == 0) { // edge case both circles are vertically aligned
      angle_degrees = secondMarker.centerX > circle.centerX ? 90.0 : -90.0; // TODO: Test this is the correct way... pure guess right now
    } else {
      angle_degrees = atan2(rise, run)*180.0 / CV_PI;

      double avg_x = (secondMarker.centerX + firstMarker.centerX)/2;
      if (avg_x > circle.centerX) { // FIXME: I don't think this is 100% accurate...
        angle_degrees += 180.0;
      }
    }

    cv::Mat rotatedHRCode;
    cv::Mat detectedHRCode(src_gray, Rect(circle.centerX-circle.radiusPx, circle.centerY-circle.radiusPx, circle.radiusPx*2, circle.radiusPx*2));
    cv::Mat rotationMatrix = cv::getRotationMatrix2D(Point2f(circle.radiusPx,circle.radiusPx), angle_degrees, 1.0);
    warpAffine(detectedHRCode, rotatedHRCode, rotationMatrix, detectedHRCode.size());

    string filename = nextFilename(DETECTED_CODES_BASE_PATH, "detected_code", ".jpg");
    string imgFilename = DETECTED_CODES_BASE_PATH + filename;
    imwrite(imgFilename, rotatedHRCode);
    HRCode codePos(rotatedHRCode, filename, circle.centerX, circle.centerY, pixelsPerMm); 
    detectedCodes.push_back(codePos);

  }

  //for( size_t i = 0; i < hierarchy.size(); i++ ) {

  //  if (!contourIsCircle[i]) { continue; }
  //  
  //  float scale = radius[i] / HRCODE_OUTER_RADIUS; // pixels per mm
  //  std::cout << "Detected a contour with a radius of " << radius[i] << " pixels. Scale: " << scale << std::endl;
  //  
  //  // TODO: Return an error message with the reason why it did not detect instead of logging stuff like this.
  //  //BOOST_LOG_TRIVIAL(debug) << "Checking circle " << i;

  //  int child = hierarchy[i][2];
  //  if (child < 0) { std::cout << "Found a circle but it has no children..." << std::endl; continue; }
  //  
  //  //BOOST_LOG_TRIVIAL(debug) << "Child found.";
  // 
  //  float insideRadius = radius[child] / scale; // mm

  //  bool correctSize = abs(insideRadius - HRCODE_INNER_RADIUS)/HRCODE_INNER_RADIUS < 0.2;
  //  if (!correctSize) { std::cout << "Expected inner dia to be " << HRCODE_INNER_RADIUS << ", but was: " << insideRadius << std::endl; continue; }
  //  
  //  int firstMarker = findNextMarker(hierarchy[child][2], hierarchy, contourIsCircle, centers, radius, child, scale);
  //  if (firstMarker < 0) { std::cout << "Did not find a first marker" << std::endl; continue; }
  //  
  //  int secondMarker = findNextMarker(hierarchy[firstMarker][0], hierarchy, contourIsCircle, centers, radius, child, scale);
  //  if (secondMarker < 0) { std::cout << "Did not find a second marker" << std::endl; continue; }
  //  
  //  int thirdInnerCircle = findNextMarker(hierarchy[secondMarker][0], hierarchy, contourIsCircle, centers, radius, child, scale);
  //  if (thirdInnerCircle >= 0) { std::cout << "Should not have detected a third inner circle. Aborting..." << std::endl; continue; }
  //  
  //  //cout << "Detected HRCode!" << endl;
  //  // Calculate angle
  //  double rise = centers[secondMarker].y - centers[firstMarker].y;
  //  double run = centers[secondMarker].x - centers[firstMarker].x;
  //  double angle_degrees;
  //  if (run == 0) { // edge case both circles are vertically aligned
  //    angle_degrees = centers[secondMarker].x > centers[i].x ? 90.0 : -90.0; // TODO: Test this is the correct way... pure guess right now
  //  } else {
  //    angle_degrees = atan2(rise, run)*180.0 / CV_PI;

  //    double avg_x = (centers[secondMarker].x + centers[firstMarker].x)/2;
  //    if (avg_x > centers[i].x) { // FIXME: I don't think this is 100% accurate...
  //      angle_degrees += 180.0;
  //    }
  //  }

  //  //BOOST_LOG_TRIVIAL(debug) << "angle_degrees: "  << angle_degrees;
 
  //  cv::Mat rotatedHRCode;
  //  cv::Mat detectedHRCode(src_gray, Rect(centers[i].x-radius[i], centers[i].y-radius[i], radius[i]*2, radius[i]*2));
  //  cv::Mat rotationMatrix = cv::getRotationMatrix2D(Point2f(radius[i],radius[i]), angle_degrees, 1.0);
  //  warpAffine(detectedHRCode, rotatedHRCode, rotationMatrix, detectedHRCode.size());

  //  string filename = nextFilename(DETECTED_CODES_BASE_PATH, "detected_code", ".jpg");
  //  string imgFilename = DETECTED_CODES_BASE_PATH + filename;
  //  imwrite(imgFilename, rotatedHRCode);
  //  HRCode codePos(rotatedHRCode, filename, centers[i].x, centers[i].y, scale); 
  //  detectedCodes.push_back(codePos);
  //}

  imwrite("tmp/lastCapture.jpg", src); // DEBUG ONLY
  imwrite("tmp/lastGray.jpg", src_gray); // DEBUG ONLY
  imwrite("tmp/lastCirclesDrawing.jpg", circlesDrawing); // DEBUG ONLY
  imwrite("tmp/lastContoursDrawing.jpg", contoursDrawing); // DEBUG ONLY
  imwrite("tmp/lastCannyDrawing.jpg", canny_output); // DEBUG ONLY

  //resize(drawing, drawing, Size(drawing.cols*2, drawing.rows*2), 0, 0, INTER_AREA);
  //imshow( "Contours", drawing );
  //waitKey(0);
}
