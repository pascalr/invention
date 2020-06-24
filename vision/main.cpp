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
 
    /* 
    // Calculate angle
    double rise = circles[1][1] - circles[0][1];
    double run = circles[1][0] - circles[0][0];
    double slope = rise / run;
    double angle_radians = atan(slope);
    double angle_degrees = angle_radians*180.0 / CV_PI;*/
  
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
    //cout << "Norm is: " + norm;
  }
  return isACircle;
}

bool isBigCircle(vector<Point> contours, Point2f center, float radius, float epsilon, float minRadius) {
    cout << "Radius is: " << radius << endl;
  return radius > minRadius && isCircle(contours, center, radius, epsilon);
}

namespace ContourDetector {
  Mat src_gray;
  int thresh = 100;
  RNG rng(12345);
  const char* source_window = "Source";
  const int max_thresh = 255;

  void refresh(int, void* ) {
    Mat canny_output;
    Canny( src_gray, canny_output, thresh, thresh*2 );
    vector<vector<Point> > contours;
    findContours( canny_output, contours, RETR_TREE, CHAIN_APPROX_SIMPLE );
    //vector<vector<Point> > contours_poly( contours.size() );
    //vector<Rect> boundRect( contours.size() );
    vector<Point2f>centers( contours.size() );
    vector<float>radius( contours.size() );
    for( size_t i = 0; i < contours.size(); i++ )
    {
        //approxPolyDP( contours[i], contours_poly[i], 3, true );
        //boundRect[i] = boundingRect( contours_poly[i] );
        //boundRect[i] = boundingRect( contours[i] );
        //minEnclosingCircle( contours_poly[i], centers[i], radius[i] );
        minEnclosingCircle( contours[i], centers[i], radius[i] );
    }
    Mat drawing = Mat::zeros( canny_output.size(), CV_8UC3 );
    for( size_t i = 0; i< contours.size(); i++ )
    {
        Scalar color = Scalar( rng.uniform(0, 256), rng.uniform(0,256), rng.uniform(0,256) );
        //drawContours( drawing, contours_poly, (int)i, color );
        //drawContours( drawing, contours, (int)i, color );
        //rectangle( drawing, boundRect[i].tl(), boundRect[i].br(), color, 2 );
        //if (isCircle(contours[i], centers[i], radius[i], 2)) {
        if (isBigCircle(contours[i], centers[i], radius[i], 0.2, 6)) {
          circle( drawing, centers[i], (int)radius[i], color, 2 );
        }
    }
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
  cout << "0";
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
