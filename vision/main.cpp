#include <opencv2/opencv.hpp>
#include "opencv2/imgproc.hpp"
#include "opencv2/highgui.hpp"
#include <iostream>
#include <cmath>

using namespace cv;
using namespace std;

// TODO: Faire des calculs
// 640X480
// 60deg, 8po de profond => Donc on voit une largeur de 8po
// Mon sticker va mesure 32mm, donc un peut plus de 1 po.
// 640 / 8 = 80 pixels pour le gros cercle

// Using a namespace before of createTrackbar, I don't want to put generic variable too much.
namespace CannyDetector {
  
  Mat src, src_gray;
  Mat dst, detected_edges;
  int lowThreshold = 0;
  const int max_lowThreshold = 100;
  const int lowThresholdRatio = 3;
  const int kernel_size = 3;
  const char* window_name = "Edge Map";
  
  static void refresh(int, void*) {
    blur( src_gray, detected_edges, Size(3,3) );
    Canny( detected_edges, detected_edges, lowThreshold, lowThreshold*lowThresholdRatio, kernel_size );
    dst = Scalar::all(0);
    src.copyTo( dst, detected_edges);
    imshow( window_name, dst );
  }
  
  int run(Mat& frame) {
    src = frame;
  
    dst.create( src.size(), src.type() );
    cvtColor( src, src_gray, COLOR_BGR2GRAY );
    namedWindow( window_name, WINDOW_AUTOSIZE );
    createTrackbar( "Min Threshold:", window_name, &lowThreshold, max_lowThreshold, refresh );
    refresh(0, 0);
    waitKey(0);
    return 0;
  }

}

// Using a namespace before of createTrackbar, I don't want to put generic variable too much.
namespace HoughCircleDetector {
 
  Mat originalFrame; 
  int param1 = 200; // aka cannyThresholdInitialValue
  const int max_param1 = 255;
  int param2 = 92; // aka accumulatorThresholdInitialValue
  const int max_param2 = 200;
  const char* window_name = "Hough Circle";

  static void refresh(int, void*) {
    // Find the two circles.
    vector<Vec3f> circles;
    
    // param1 and param2 cannot be 0
    param1 = std::max(param1, 1);
    param2 = std::max(param2, 1);
  
    Mat frame = originalFrame.clone(); 
    Mat gray;
    cvtColor(frame, gray, COLOR_BGR2GRAY);
    // Parameter values are very much obtained by trial and error
    HoughCircles(gray, // input
        circles, // output
        HOUGH_GRADIENT, // method
        1, // dp
        frame.rows/16, // minimum distance
        param1, // param1
        param2, // param2
        0, // minimum radius
        0); // maximum radius

    //medianBlur(frame, frame, 5);
  
    cout << "Found " << circles.size() << " circles." << endl;
  
    // draw the detected circles
    for( size_t i = 0; i < circles.size(); i++ ) {
        Vec3i c = circles[i];
        Point center = Point(c[0], c[1]);
        // circle center
        circle( frame, center, 10, Scalar(0,0,255), FILLED, LINE_8);
        // circle outline
        int radius = c[2];
        circle( frame, center, radius, Scalar(0,0,255), 10, LINE_8);
    }
 
 
    resize(frame, frame, Size(640,480), 0, 0, INTER_CUBIC);
    imshow(window_name, frame);
  }

  int run(Mat& theFrame) {

    originalFrame = theFrame;
    
    namedWindow( window_name, WINDOW_AUTOSIZE );
    createTrackbar( "Param1:", window_name, &param1, max_param1, refresh );
    createTrackbar( "Param2:", window_name, &param2, max_param2, refresh );
    refresh(0, 0);
  
    waitKey(0);
    return 0;
  }
}

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

namespace ContourDetector {
  Mat src_gray;
  int thresh = 100;
  RNG rng(12345);
  const char* source_window = "Source";
  const int max_thresh = 255;

  int findNextCircle(int i, vector<cv::Vec4i> hierarchy, vector<bool> contourIsCircle) {
    if (i < 0) return -1;
    if (contourIsCircle[i]) {
      return i;
    } else {
      return findNextCircle(hierarchy[i][0], hierarchy, contourIsCircle);
    }
  }

  void refresh(int, void* ) {
    Mat canny_output;
    Canny( src_gray, canny_output, thresh, thresh*2 );

    // https://stackoverflow.com/questions/43852023/detecting-small-circles-using-houghcircles-opencv
    // Use erosion and dilation combination to eliminate false positives.
    // In this case the text Q0X could be identified as circles but it is not.
    // mask = cv2.erode(mask, kernel, iterations=6)
    // mask = cv2.dilate(mask, kernel, iterations=3)
    // closing = cv2.morphologyEx(mask, cv2.MORPH_OPEN, kernel)

    vector<vector<Point>> contours;
    vector<cv::Vec4i> hierarchy;
    findContours( canny_output, contours, hierarchy, RETR_TREE, CHAIN_APPROX_SIMPLE );
    //vector<vector<Point> > contours_poly( contours.size() );
    //vector<Rect> boundRect( contours.size() );
    vector<Point2f> centers( contours.size() );
    vector<float> radius( contours.size() );
    vector<bool> contourIsCircle( contours.size() );

    Mat drawing = Mat::zeros( canny_output.size(), CV_8UC3 );

    for( size_t i = 0; i < contours.size(); i++ )
    {
        //approxPolyDP( contours[i], contours_poly[i], 3, true );
        //boundRect[i] = boundingRect( contours_poly[i] );
        //boundRect[i] = boundingRect( contours[i] );
        //minEnclosingCircle( contours_poly[i], centers[i], radius[i] );
        minEnclosingCircle( contours[i], centers[i], radius[i] );
        Scalar color = Scalar( rng.uniform(0, 256), rng.uniform(0,256), rng.uniform(0,256) );
        if (isBigCircle(contours[i], centers[i], radius[i], 0.2, 6)) {
          contourIsCircle[i] = true;
          circle( drawing, centers[i], (int)radius[i], color, 2 );
        } else {
          contourIsCircle[i] = false; // OPTIMIZE: Default vector to false to avoid doing that
        }
    }

    for( size_t i = 0; i < hierarchy.size(); i++ ) {

      // FIXME only check circles not all contours

      int child = hierarchy[i][2];
      if (child < 0) continue;

      //int firstInnerCircle = hierarchy[child][2];
      int firstInnerCircle = findNextCircle(hierarchy[child][2], hierarchy, contourIsCircle);
      if (firstInnerCircle < 0 || !contourIsCircle[child]) continue;

      //int secondInnerCircle = hierarchy[firstInnerCircle][0];
      int secondInnerCircle = findNextCircle(hierarchy[firstInnerCircle][0], hierarchy, contourIsCircle);
      if (secondInnerCircle < 0) continue;

      //int thirdInnerCircle = hierarchy[secondInnerCircle][0];
      int thirdInnerCircle = findNextCircle(hierarchy[secondInnerCircle][0], hierarchy, contourIsCircle);
      if (thirdInnerCircle >= 0) continue; // No third expected... maybe do better than that latter to remove false positives..
        
      cout << "Possible candidate!!!" << endl;

      // All relatives because actual size vary based on distance of camera to sticker.
      float borderThickness = 2; // mm
      float insideDiameter = 30; // mm
      float smallDotDiameter = 4.86; // mm
      float outsideDiameter = insideDiameter + 2*borderThickness; // mm

      float expectedRatio = outsideDiameter / insideDiameter;
      float actualRatio = radius[i] / radius[child];
      bool perimeterDetected = abs(actualRatio - expectedRatio)/expectedRatio < 0.2;

      float dotExpectedRatio = outsideDiameter / smallDotDiameter;
      float dotActualRatio = radius[i] / radius[firstInnerCircle];
      bool dotDetected = abs(dotActualRatio - dotExpectedRatio)/dotExpectedRatio < 0.2;

      float dotActualRatio2 = radius[i] / radius[secondInnerCircle];
      bool dotDetected2 = abs(dotActualRatio2 - dotExpectedRatio)/dotExpectedRatio < 0.2;

      cout << "perimeterDetected: " << perimeterDetected << endl;
      cout << "firstDotDetected: " << dotDetected << endl;
      cout << "secondDotDetected: " << dotDetected2 << endl;
      cout << "dotExpectedRatio: " << dotExpectedRatio << endl;
      cout << "dotActualRatio: " << dotActualRatio << endl;

      if (perimeterDetected && dotDetected && dotDetected2) {
        cout << "Detected HRCode!!!" << endl;
        // Calculate angle
        double rise = centers[secondInnerCircle].y - centers[firstInnerCircle].y;
        double run = centers[secondInnerCircle].x - centers[firstInnerCircle].x;
        double slope = rise / run;
        double angle_radians = atan(slope);
        double angle_degrees = angle_radians*180.0 / CV_PI;
        cout << "angle_degrees: "  << angle_degrees << endl;
 
        circle( drawing, centers[i], 4, Scalar(0,0,255), FILLED );

        //Mat detectedHRCode(src_gray, Rect(centers[i].x-radius[i], centers[i].y-radius[i], centers[i].x+radius[i], centers[i].y+radius[i]));
        Mat rotatedHRCode;
        Mat detectedHRCode(src_gray, Rect(centers[i].x-radius[i], centers[i].y-radius[i], radius[i]*2, radius[i]*2));
        Mat rotationMatrix = cv::getRotationMatrix2D(Point2f(radius[i],radius[i]), angle_degrees, 1.0);
        warpAffine(detectedHRCode, rotatedHRCode, rotationMatrix, detectedHRCode.size());

        //cv::normalize(leftROI, leftROI, 0, 1, cv::NORM_MINMAX);  //to view
        std::string title = "detectedHRCode";
        title += std::to_string(i);
        imshow("detectedHRCode",rotatedHRCode);
      }
    }
    
    /*for( size_t i = 0; i< contours.size(); i++ )
    {
        Scalar color = Scalar( rng.uniform(0, 256), rng.uniform(0,256), rng.uniform(0,256) );
        //drawContours( drawing, contours_poly, (int)i, color );
        //drawContours( drawing, contours, (int)i, color );
        //rectangle( drawing, boundRect[i].tl(), boundRect[i].br(), color, 2 );
        //if (isCircle(contours[i], centers[i], radius[i], 2)) {
        if (isBigCircle(contours[i], centers[i], radius[i], 0.2, 6)) {
          circle( drawing, centers[i], (int)radius[i], color, 2 );
        }
    }*/
    imshow( "Contours", drawing );
  }

  int run(Mat& src) {
    cvtColor( src, src_gray, COLOR_BGR2GRAY );
    blur( src_gray, src_gray, Size(3,3) ); // Remove noise
    namedWindow( source_window );
    imshow( source_window, src );
    createTrackbar( "Canny thresh:", source_window, &thresh, max_thresh, refresh );
    refresh( 0, 0 );
    waitKey();
    return 0;
  }
  
}

int main(int argc, char** argv ) {
  Mat mat;
  if (argc == 2) {
    mat = imread( argv[1] );
    cout << "Input image is " << mat.size() << endl;
  } else {
    mat = Mat(200, 200, CV_8UC3, Scalar(255,255,255));
    circle(mat, Point(100,100), 40, Scalar(0,0,0), 2, LINE_8);
    circle(mat, Point(120,80), 5, Scalar(0,0,0), FILLED, LINE_8);
    circle(mat, Point(80,80), 5, Scalar(0,0,0), FILLED, LINE_8);
  }

  //HoughCircleDetector::run(mat);
  //CannyDetector::run(mat);
  ContourDetector::run(mat);
}
