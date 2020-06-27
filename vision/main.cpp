#include <opencv2/opencv.hpp>
#include "opencv2/imgproc.hpp"
#include "opencv2/highgui.hpp"
#include <iostream>
#include <cmath>
#include <stdlib.h>
#include <stdio.h>

#include "common.h"

#include "hr_code.hpp"

using namespace cv;
using namespace std;

// TODO: Imprimer des sticker avec openCV, comme ca je peux controller les dimensions exactement et facilement.
// Aussi, je vais pouvoir faire plein de test et de training ainsi.

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

namespace ErosionDilatation {
  /// Global variables
  Mat src, erosion_dst, dilation_dst;
  
  int erosion_elem = 0;
  int erosion_size = 0;
  int dilation_elem = 0;
  int dilation_size = 0;
  int const max_elem = 2;
  int const max_kernel_size = 21;
  
  /** Function Headers */
  void Erosion( int, void* );
  void Dilation( int, void* );
  
  /** @function main */
  int run(Mat& image)
  {
    src = image;
    /// Create windows
    namedWindow( "Erosion Demo", WINDOW_AUTOSIZE );
    namedWindow( "Dilation Demo", WINDOW_AUTOSIZE );
    moveWindow( "Dilation Demo", src.cols, 0 );
  
    /// Create Erosion Trackbar
    createTrackbar( "Element:\n 0: Rect \n 1: Cross \n 2: Ellipse", "Erosion Demo",
                    &erosion_elem, max_elem,
                    Erosion );
  
    createTrackbar( "Kernel size:\n 2n +1", "Erosion Demo",
                    &erosion_size, max_kernel_size,
                    Erosion );
  
    /// Create Dilation Trackbar
    createTrackbar( "Element:\n 0: Rect \n 1: Cross \n 2: Ellipse", "Dilation Demo",
                    &dilation_elem, max_elem,
                    Dilation );
  
    createTrackbar( "Kernel size:\n 2n +1", "Dilation Demo",
                    &dilation_size, max_kernel_size,
                    Dilation );
  
    /// Default start
    Erosion( 0, 0 );
    Dilation( 0, 0 );
  
    waitKey(0);
    return 0;
  }
  
  /**  @function Erosion  */
  void Erosion( int, void* )
  {
    int erosion_type;
    if( erosion_elem == 0 ){ erosion_type = MORPH_RECT; }
    else if( erosion_elem == 1 ){ erosion_type = MORPH_CROSS; }
    else if( erosion_elem == 2) { erosion_type = MORPH_ELLIPSE; }
  
    Mat element = getStructuringElement( erosion_type,
                                         Size( 2*erosion_size + 1, 2*erosion_size+1 ),
                                         Point( erosion_size, erosion_size ) );
  
    /// Apply the erosion operation
    erode( src, erosion_dst, element );
    imshow( "Erosion Demo", erosion_dst );
  }
  
  /** @function Dilation */
  void Dilation( int, void* )
  {
    int dilation_type;
    if( dilation_elem == 0 ){ dilation_type = MORPH_RECT; }
    else if( dilation_elem == 1 ){ dilation_type = MORPH_CROSS; }
    else if( dilation_elem == 2) { dilation_type = MORPH_ELLIPSE; }
  
    Mat element = getStructuringElement( dilation_type,
                                         Size( 2*dilation_size + 1, 2*dilation_size+1 ),
                                         Point( dilation_size, dilation_size ) );
    /// Apply the dilation operation
    dilate( src, dilation_dst, element );
    imshow( "Dilation Demo", dilation_dst );
  }
}

namespace ContourDetector {
  Mat src_gray;
  int thresh = 100;
  const char* source_window = "Source";
  const int max_thresh = 255;

  void refresh(int, void* ) {
    /*Mat dilate_output;
    Mat element = getStructuringElement( MORPH_RECT, Size(3,3), Point(1, 1) );
    dilate( src_gray, dilate_output, element );*/

    HRCodeParser parser(0.2, 0.2);
    vector<Mat> hr_codes;
    parser.findHRCode(src_gray, hr_codes, thresh);

    for( size_t i = 0; i < hr_codes.size(); i++ ) {
      Mat m = hr_codes[i];

      cvtColor( m, m, COLOR_GRAY2BGR );
      parseHRCode(m);

      string title = string("detectedHRCode") + to_string(i) + ".png";
      imshow(title,m);
      imwrite(title,m);
    }
  }

  int run(Mat& src) {
    cvtColor( src, src_gray, COLOR_BGR2GRAY );
    blur( src_gray, src_gray, Size(3,3) ); // Remove noise
    //namedWindow( source_window );
    //imshow( source_window, src );
    //createTrackbar( "Canny thresh:", source_window, &thresh, max_thresh, refresh );
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
  //ErosionDilatation::run(mat);
}
