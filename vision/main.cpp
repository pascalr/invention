#include <opencv2/opencv.hpp>
#include "opencv2/imgproc.hpp"
#include "opencv2/highgui.hpp"
#include <iostream>
#include <cmath>

using namespace cv;
using namespace std;

namespace CannyDetector {
  
  Mat src, src_gray;
  Mat dst, detected_edges;
  int lowThreshold = 0;
  const int max_lowThreshold = 100;
  const int lowThresholdRatio = 3;
  const int kernel_size = 3;
  const char* window_name = "Edge Map";
  
  static void refresh(int, void* generic)
  {
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

  //cvtColor(frame, frame, COLOR_BGR2GRAY);

  //CommandLineParser parser( argc, argv, "{@input | fruits.jpg | input image}" );
  //src = imread( samples::findFile( parser.get<String>( "@input" ) ), IMREAD_COLOR ); // Load an image
  //if( src.empty() )
  //{
  //  std::cout << "Could not open or find the image!\n" << std::endl;
  //  std::cout << "Usage: " << argv[0] << " <Input image>" << std::endl;
  //  return -1;
  //}



int mainHoughCircles(Mat& frame) {
  
  // Find the two circles.
  vector<Vec3f> circles;

  Mat gray;
  cvtColor(frame, gray, COLOR_BGR2GRAY);
  // Parameter values are very much obtained by trial and error
  HoughCircles(gray, // input
      circles, // output
      HOUGH_GRADIENT, // method
      1, // dp
      frame.rows/16, // minimum distance
      200, // param1
      100, // param2
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

  /*Point2d centre1(circles[0][0], circles[0][1]);
  Point2d centre2(circles[1][0], circles[1][1]);

  circle(frame, centre1, 10, Scalar(0, 0, 0), 3);
  circle(frame, centre2, 10, Scalar(0, 0, 0), 3);

  // Calculate angle
  double rise = circles[1][1] - circles[0][1];
  double run = circles[1][0] - circles[0][0];
  double slope = rise / run;
  double angle_radians = atan(slope);
  double angle_degrees = angle_radians*180.0 / CV_PI;*/

  resize(frame, frame, Size(640,480), 0, 0, INTER_CUBIC);
  imshow("Hough Circles", frame);
  //createTrackbar( "Min Threshold:", window_name, &lowThreshold, max_lowThreshold, CannyThreshold );

  waitKey(0);
  return 0;
}

int main(int argc, char** argv ) {
  cout << "0";
  Mat mat;
  if (argc == 2) {
    mat = imread( argv[1] );
  } else {
    mat = Mat(200, 200, CV_8UC3, Scalar(255,255,255));
    circle(mat, Point(100,100), 60, Scalar(0,0,0), FILLED, LINE_8);
  }

  //mainCannyThreshold(mat);
  //mainHoughCircles(mat);
  
  CannyDetector::run(mat);
}
