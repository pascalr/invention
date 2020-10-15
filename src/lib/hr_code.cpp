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

  // Get the sub-matrices (minors) for every character.
}

// Adjusts contrast or brightness I don't know. Darkens too white images and whitens too dark images.
void adjustBrightness(cv::Mat& src, cv::Mat& dest) {
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



// Find characters with contours does not work because they are too close together
void parseText(vector<string>& parsedLines, cv::Mat gray) {
  
  //resize(gray, gray, Size(gray.cols*2, gray.rows*2), 0, 0, INTER_AREA);
  
  cv::Mat dst = gray.clone(); 
  imwrite("tmp/hrcode/parser/lastText.jpg", dst); // DEBUG ONLY
 
  blur(dst, dst, Size(3,3)); // Remove noise
  imwrite("tmp/hrcode/parser/lastBlur.jpg", dst); // DEBUG ONLY

  adjustBrightness(dst,dst);
  imwrite("tmp/hrcode/parser/lastBrightness.jpg", dst); // DEBUG ONLY

  vector<cv::Mat> lines;
  extractLines(lines, dst);
  imwrite("tmp/hrcode/parser/lastJarId.jpg", lines[0]); // DEBUG ONLY

  parsedLines.push_back(parseDigitLine(lines[0]));
  //parsedLines.push_back(parser.parseLine(lines[1]));
  //parsedLines.push_back(parser.parseLine(lines[2]));
  //parsedLines.push_back(parser.parseDigitLine(lines[3]));
  for (const string& line : parsedLines) {
    cout << "Detected code: " << line << endl;
  }

  //hconcat(sideBySide, dst, sideBySide);

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
      return cv::norm(center - c.center);
      //return sqrt(pow(center_x - c.center_x, 2) + pow(center_y - c.center_y, 2));
    }
    double distanceMm(CircleDetected c, double pixelsPerMm) {
      return distancePx(c) / pixelsPerMm;
    }
    double radiusMm(double pixelsPerMm) {
      return radius_px / pixelsPerMm;
    }
    int id;
    double radius_px;
    // Point2f center; should be this...
    // double res = cv::norm(a-b); And I should use this
    Point2f center;
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

// Take the average of all points to get the center.
// Take the average of all the distances between the points and the center to get radius.
void averageEnclosingCircle(const vector<Point> &contour, Point2f &center, float &radius) {

  center.x = 0;
  center.y = 0;
  for (const Point& pt : contour) {
    center.x += pt.x;
    center.y += pt.y;
  }
  center.x /= contour.size();
  center.y /= contour.size();

  radius = 0;
  for (const Point& pt : contour) {
    radius += sqrt(pow(pt.x - center.x, 2) + pow(pt.y - center.y, 2));
  }
  radius /= contour.size();
}

class ArcDetected {
  public:
    double angle() {
      // Source: https://stackoverflow.com/questions/1878907/the-smallest-difference-between-2-angles
      double a = end_angle_deg - start_angle_deg;
      a += (a>180) ? -360 : (a<-180) ? 360 : 0;
      return a;
    }
    Point2f center;
    double radius_px;
    double start_angle_deg;
    double end_angle_deg;
};

// OPTIMIZE: merge the arcs based on the start and end angles, not just the centers.

// Source: https://stackoverflow.com/questions/15888180/calculating-the-angle-between-points
double angleTwoPointDeg(const Point& src1, const Point& src2, const Point2f& center) {

  Point2f v1 = Point2f(src1) - center;
  Point2f v2 = Point2f(src2) - center;

  double cosAngle = v1.dot(v2) / (cv::norm(v1) * cv::norm(v2));
  if (cosAngle > 1.0)
      return 0.0;
  else if (cosAngle < -1.0)
      return 180.0;
  return std::acos(cosAngle) * 180.0 / PI;
}

bool isArc(vector<Point> points, ArcDetected &a) {

  // There must be a minimum amount of points for fitEllipse (6), which is find because we don't want too small contours.
  if (points.size() < 10) return false;

  // There are many fitEllipse available. OPTIMIZE: Test which one is better for my use case.
  // Ah maybe try it for each one every time and see which one gives me the best fit.
  //cv::fitEllipse
  //cv::fitEllipseAMS
  //cv::fitEllipseDirect
  RotatedRect r = cv::fitEllipse(points);
  // The detected enclosing rectangle must be close to a square because we want to detet cercles.
  // HARCODED. Smaller side must be at least 80% of the bigger side.
  if (min(r.size.height, r.size.width) / max(r.size.height, r.size.width) < 0.8) return false; 

  double minRadius = 9999999999999999.9;
  double maxRadius = 0.0;
  for (Point& pt : points) {
    // The radius is the distance between the point and the center.
    double radius = cv::norm(r.center - Point2f(pt));
    minRadius = min(radius, minRadius);
    maxRadius = max(radius, maxRadius);
  }
 
  // Doing it this way in order to get the correct angle of the two possible
  double sumAngle = 0.0;
  for (size_t i = 0; i < points.size() - 1; i++) {
    sumAngle += angleTwoPointDeg(points[i], points[i+1], r.center);
  }

  // HARCODED. Smallest radius must be at least 80% of the biggest radius.
  if (minRadius / maxRadius < 0.8) return false; 

  // What is better for the radius?
  // a.radius_px = (r.size.height + r.size.width) / 4.0;
  // a.radius_px = meanRadius;

  a.center = r.center;
  a.radius_px = (r.size.height + r.size.width) / 4.0;

  Point2f firstVect = Point2f(*points.begin()) - a.center;
  a.start_angle_deg = atan2(firstVect.y, firstVect.x)*180.0/PI;

  //Point2f lastVect = Point2f(points.back()) - a.center;
  //a.end_angle_deg = atan2(lastVect.y, lastVect.x)*180.0/PI;

  a.end_angle_deg = a.start_angle_deg + sumAngle;

  double angle = angleTwoPointDeg(*points.begin(), points.back(), a.center);

  cout << "Arc: (" << a.center.x << "," << a.center.y << ") r" << a.radius_px << " px, start=" << a.start_angle_deg << ", end=" << a.end_angle_deg << endl;
  cout << "Sum angle: " << sumAngle << endl;
  cout << "Angle: " << angle << endl;

  // HARDCODED. A minimum of 30 degrees is required
  //if (a.angle() < 30.0) {
  //}

  return true;
}

// OK, new algorithm needed... I cannot use the hierarchies...
// Because when only an arc of a circle is detected, than it is not considered a container.
// But for me an arc is enough. The contours detector offen detects multiple edges for the same circle edge.
//
// We find all the circles.
// Then for each circle, we assume it is the outer perimeter.
// We then check if it has an inner perimeter.
// We then check if it has two and only two markers.
void findHRCodes(cv::Mat& original, vector<HRCode> &detectedCodes) {

  cv::Mat src = original.clone();
  imwrite("tmp/hrcode/finder/lastCapture.jpg", src); // DEBUG ONLY

  cvtColor(src, src, COLOR_BGR2GRAY );
  imwrite("tmp/hrcode/finder/lastGray.jpg", src); // DEBUG ONLY
  Mat srcGray = src.clone();

  // Adjust brighteness to average this way always similar process
  adjustBrightness(src, src);
  imwrite("tmp/hrcode/finder/lastBrightness.jpg", src); // DEBUG ONLY

  // https://docs.opencv.org/master/d4/d13/tutorial_py_filtering.html
  // maybe medianBlur could be used instead of blur
  // cv.bilateralFilter() is highly effective in noise removal while keeping edges sharp.

  ////blur(src, src, Size(3,3) ); // Remove noise
  ////medianBlur(src, src, 5);
  //bilateralFilter(src.clone(), src, 9, 75, 75);
  //imwrite("tmp/hrcode/finder/lastBlur.jpg", src); // DEBUG ONLY

  // http://datahacker.rs/004-how-to-smooth-and-sharpen-an-image-in-opencv/
  double mydata[]={0, -1,  0,
                  -1,  9, -1,
                   0, -1,  0};
  cv::Mat filterKernel(3,3,CV_64F,mydata);
  filter2D(src,src,-1,filterKernel);
  imwrite("tmp/hrcode/finder/lastFilter2D.jpg", src); // DEBUG ONLY

  cv::Mat kernel;

  int beforeDilateSize = 2; // HARDCODED.
  kernel = getStructuringElement( MORPH_RECT, Size( 2*beforeDilateSize + 1, 2*beforeDilateSize+1 ), Point( beforeDilateSize, beforeDilateSize ) );
  erode(src, src, kernel );
  imwrite("tmp/hrcode/finder/lastErodeBefore.jpg", src); // DEBUG ONLY
  dilate(src, src, kernel );
  imwrite("tmp/hrcode/finder/lastDilateBefore.jpg", src); // DEBUG ONLY

  // Canny does a gaussian internally, I wanted to see the result.
  cv::Mat gaussianDest;
  GaussianBlur(src, gaussianDest, Size(5,5), 0);
  imwrite("tmp/hrcode/finder/lastCannyGaussian.jpg", gaussianDest); // DEBUG ONLY

  cv::bitwise_not(src, src);
  filter2D(src,src,-1,filterKernel);
  cv::bitwise_not(src, src);
  imwrite("tmp/hrcode/finder/lastInvertedFilter2D.jpg", src); // DEBUG ONLY
  
  int thresh = 128; // HARDCODED
  Canny(src, src, thresh, thresh*2 );
  imwrite("tmp/hrcode/finder/lastCanny.jpg", src); // DEBUG ONLY

  // https://stackoverflow.com/questions/35922687/pre-processing-image-before-applying-canny-edge-detection
  // Convert the image to grayscale
  // Apply a bilateral filter
  // Run the Canny edge detection process
  // Apply two more bilateral filters to remove any noise
  // Apply a dilation filter to 'plug' any holes in the edges
  
  int dilateSize = 1; // HARDCODED.
  kernel = getStructuringElement( MORPH_RECT, Size( 2*dilateSize + 1, 2*dilateSize+1 ), Point( dilateSize, dilateSize ) );
  dilate(src, src, kernel );
  erode(src, src, kernel );
  imwrite("tmp/hrcode/finder/lastErrodeAndDilateAfter.jpg", src); // DEBUG ONLY

  //cv::adaptiveThreshold(src, src, 255, cv::ADAPTIVE_THRESH_MEAN_C, cv::THRESH_BINARY,11,2);
  ////cv::adaptiveThreshold(src, src, 255, cv::ADAPTIVE_THRESH_GAUSSIAN_C, cv::THRESH_BINARY,11,2);
  //imwrite("tmp/hrcode/finder/lastThreshold.jpg", src); // DEBUG ONLY

  // ellipse2Poly(), maybe use this

  RNG rng(12345);
  vector<vector<Point>> contours;
  vector<cv::Vec4i> hierarchy;
  findContours( src, contours, hierarchy, RETR_TREE, CHAIN_APPROX_SIMPLE );
  vector<Point2f> centers( contours.size() );
  vector<float> radius( contours.size() );
  //vector<bool> contourIsCircle( contours.size(), false );
  vector<CircleDetected> manyCircles;

  cv::Mat circlesDrawing = original.clone();
  cv::Mat mergedDrawing = original.clone();
  cv::Mat arcsDrawing = original.clone();
  cv::Mat contoursDrawing = original.clone();
  //cv::Mat circlesDrawing = cv::Mat::zeros( canny_output.size(), CV_8UC3 ); // DEBUG ONLY
  //cv::Mat contoursDrawing = cv::Mat::zeros( canny_output.size(), CV_8UC3 ); // DEBUG ONLY

  for( size_t i = 0; i < contours.size(); i++ ) {

    //Scalar color = Scalar( rng.uniform(0, 256), rng.uniform(0,256), rng.uniform(0,256) ); // DEBUG ONLY
    //drawContours( contoursDrawing, contours, i, color, 3, cv::LINE_8, hierarchy ); // DEBUG ONLY

    //ArcDetected arc;
    //if (!isArc(contours[i], arc)) continue;
    //
    //CircleDetected c;
    //c.id = i;
    //c.radius_px = arc.radius_px;
    //c.center_x = arc.center.x;
    //c.center_y = arc.center.y;
    //circles.push_back(c);
    //
    //cv::ellipse( arcsDrawing, arc.center, Size(arc.radius_px, arc.radius_px), 0.0, arc.start_angle_deg, arc.end_angle_deg, color, 3, LINE_8);
    //cv::circle( circlesDrawing, Point2f(c.center_x, c.center_y), (int)c.radius_px, color, 3 ); // DEBUG ONLY

    // ------------------------------------------------------------------------------

    //minEnclosingCircle( contours[i], centers[i], radius[i] );
    averageEnclosingCircle(contours[i], centers[i], radius[i]);

    Scalar color = Scalar( rng.uniform(0, 256), rng.uniform(0,256), rng.uniform(0,256) ); // DEBUG ONLY
    drawContours( contoursDrawing, contours, i, color, 3, cv::LINE_8, hierarchy ); // DEBUG ONLY

    // FIXME: Minimum circle is 6 pixels wide, is that ok?
    if (isBigCircle(contours[i], centers[i], radius[i], 0.3, 6)) {
      //contourIsCircle[i] = true;

      CircleDetected c;
      c.id = i;
      c.radius_px = radius[i];
      c.center = centers[i];
      manyCircles.push_back(c);
      
      circle( circlesDrawing, c.center, (int)c.radius_px, color, 3 ); // DEBUG ONLY

    }
  }
      
  std::function<bool(CircleDetected,CircleDetected)> func = [](CircleDetected c1, CircleDetected c2) {
    return c1.distancePx(c2) < 10 && abs(c1.radius_px - c2.radius_px) < 4; // HARDCODED.
  };
  //std::function<bool(CircleDetected,CircleDetected)> func = [](CircleDetected c1, CircleDetected c2) {
  //  return c1.distancePx(c2) < 4; // HARDCODED.
  //};
  vector<vector<size_t>> groups = groupNearDuplicates(manyCircles, func);
  vector<CircleDetected> circles;

  // Merge the near duplicates
  for (vector<size_t> &group : groups) {
    Point2f center(0.0, 0.0);
    double radiusPx = 0.0;
    for (size_t &i : group) {
      center += manyCircles[i].center;
      radiusPx += manyCircles[i].radius_px;
    }

    CircleDetected c;
    c.id = *group.begin();
    c.radius_px = radiusPx / group.size();
    c.center = center / (float)group.size();
    circles.push_back(c);

    Scalar color = Scalar( rng.uniform(0, 256), rng.uniform(0,256), rng.uniform(0,256) ); // DEBUG ONLY
    circle( mergedDrawing, c.center, (int)c.radius_px, color, 3 ); // DEBUG ONLY
  }
 
  imwrite("tmp/hrcode/finder/lastArcsDrawing.jpg", arcsDrawing); // DEBUG ONLY
  imwrite("tmp/hrcode/finder/lastCirclesDrawing.jpg", circlesDrawing); // DEBUG ONLY
  imwrite("tmp/hrcode/finder/lastContoursDrawing.jpg", contoursDrawing); // DEBUG ONLY
  imwrite("tmp/hrcode/finder/lastMergedCirclesDrawing.jpg", mergedDrawing); // DEBUG ONLY

  for (CircleDetected& circle : circles) {

    // Assume the circle is the outer perimeter of the code
    float pixelsPerMm = circle.radius_px / HRCODE_OUTER_RADIUS;
    std::cout << "Detected a circle with a radius of " << circle.radius_px << " pixels. Pixels per mm: " << pixelsPerMm << std::endl;
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

    if (markers.size() > 2) {std::cout << "Detected too many markers. Expected only 2. Detected: " << groups.size() << std::endl; continue;}

    bool correctDistance = abs(firstMarker.distanceMm(secondMarker, pixelsPerMm) - HRCODE_MARKERS_INTERSPACE)/HRCODE_MARKERS_INTERSPACE < 0.2; // HARDCODED. 20% is a little generous
    if (!correctDistance) {std::cout << "The interspace between the two markers was not at the correct distance." << std::endl; continue;}

    cout << "Detected HRCode!" << endl;

    // Calculate angle
    double rise = secondMarker.center.y - firstMarker.center.y;
    double run = secondMarker.center.x - firstMarker.center.x;
    double angle_degrees;
    if (run == 0) { // edge case both circles are vertically aligned
      angle_degrees = secondMarker.center.x > circle.center.x ? 90.0 : -90.0; // TODO: Test this is the correct way... pure guess right now
    } else {
      angle_degrees = atan2(rise, run)*180.0 / CV_PI;

      double avg_x = (secondMarker.center.x + firstMarker.center.x)/2;
      if (avg_x > circle.center.x) { // FIXME: I don't think this is 100% accurate...
        angle_degrees += 180.0;
      }
    }

    cv::Mat rotatedHRCode;
    cv::Mat detectedHRCode(srcGray, Rect(circle.center.x-circle.radius_px, circle.center.y-circle.radius_px, circle.radius_px*2, circle.radius_px*2));
    cv::Mat rotationMatrix = cv::getRotationMatrix2D(Point2f(circle.radius_px,circle.radius_px), angle_degrees, 1.0);
    warpAffine(detectedHRCode, rotatedHRCode, rotationMatrix, detectedHRCode.size());

    string filename = nextFilename(DETECTED_CODES_BASE_PATH, "detected_code", ".jpg");
    string imgFilename = DETECTED_CODES_BASE_PATH + filename;
    imwrite(imgFilename, rotatedHRCode);
    HRCode codePos(rotatedHRCode, filename, circle.center.x, circle.center.y, pixelsPerMm); 
    detectedCodes.push_back(codePos);

  }

}


//      int dilateSize = 2; // TODO: Make this a param
//      cv::Mat kernel = getStructuringElement( MORPH_RECT, Size( 2*dilateSize + 1, 2*dilateSize+1 ), Point( dilateSize, dilateSize ) );
//      dilate(src, dest, kernel );
//
//      adaptiveThreshold(src, dest, 255, ADAPTIVE_THRESH_MEAN_C, THRESH_BINARY, 3, 5 );
//
//    
//      int erosionSize = 2; // TODO: Make this a param
//      cv::Mat kernel = getStructuringElement( MORPH_RECT, Size( 2*erosionSize + 1, 2*erosionSize+1 ), Point( erosionSize, erosionSize ) );
//      erode(src, dest, kernel );
//      
//      threshold(src, dest, 255/2.0, 255, THRESH_BINARY);
//
//// Make everything below the threshold black
//      threshold(src, dest, 80, 255, THRESH_TOZERO);
//
//// Make everything above (255-threshold) white
//      threshold(src, dest, 80, 255, THRESH_TOZERO);
//
//      double contrast = 1.1;
//      dest = src * contrast;
//
//      blur(src, dest, Size(3,3)); // Remove noise
//
//      int thresh = 20;
//      Canny(src, dest, thresh, thresh*2 );
//
//// CannyProcess should be run just before
//      RNG rng(12345);
//      vector<vector<Point>> contours;
//      vector<cv::Vec4i> hierarchy;
//      findContours( src, contours, hierarchy, RETR_TREE, CHAIN_APPROX_SIMPLE );
//
//      int idx = 0;
//      for(; idx >= 0; idx = hierarchy[idx][0] )
//      {
//          Scalar color( rand()&255, rand()&255, rand()&255 );
//          drawContours( dest, contours, idx, color, FILLED, 8, hierarchy );
//      }

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
