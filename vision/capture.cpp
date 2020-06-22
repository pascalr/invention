#include <opencv2/opencv.hpp>
#include <opencv2/core.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/highgui.hpp>
#include <iostream>
#include <stdio.h>

#include "common.h"

using namespace cv;
using namespace std;

int main(int, char**)
{
  Mat frame;
  captureVideoImage(frame);
  imwrite("output/capture.jpg", frame);
  for (;;) {
    imshow("Live", frame);
    if (waitKey(40) >= 0)
    break;
  }
  return 0;
}

/*
int main(int argc, char** argv )
{

  if ( argc != 2 )
  {
      printf("usage: DisplayImage.out <Image_Path>\n");
      return -1;
  }

  Size patternsize(7,7); //interior number of corners
  Mat img = imread( argv[1], IMREAD_UNCHANGED ); //source image
  Mat gray = imread( argv[1], IMREAD_GRAYSCALE ); //source image
  if ( !gray.data )
  {
    printf("No image data \n");
    return -1;
  }
  vector<Point2f> corners; //this will be filled by the detected corners
  
  //CALIB_CB_FAST_CHECK saves a lot of time on images
  //that do not contain any chessboard corners
  bool patternfound = findChessboardCorners(gray, patternsize, corners,
          CALIB_CB_ADAPTIVE_THRESH + CALIB_CB_NORMALIZE_IMAGE);
  //bool patternfound = findChessboardCorners( img, boardSize, ptvec, CV_CALIB_CB_ADAPTIVE_THRESH );

  cout << "There are " << corners.size() << " corners found." << endl;
  
  if(patternfound)
    cornerSubPix(gray, corners, Size(11, 11), Size(-1, -1),
      TermCriteria(2, 30, 0.1));
    //cornerSubPix(gray, corners, Size(11, 11), Size(-1, -1),
     // TermCriteria(CV_TERMCRIT_EPS + CV_TERMCRIT_ITER, 30, 0.1));
  
  drawChessboardCorners(img, patternsize, Mat(corners), patternfound);

  imwrite("output.jpg", img);
    
  namedWindow("Display Image", WINDOW_AUTOSIZE );
  imshow("Display Image", img);
  waitKey(0);
  return 0;
}
*/




















/*
#include <wiringPi.h>
int main (void)
{
  wiringPiSetup () ;
  pinMode (0, OUTPUT) ;
  for (;;)
  {
    digitalWrite (0, HIGH) ; delay (500) ;
    digitalWrite (0,  LOW) ; delay (500) ;
  }
  return 0 ;
}
*/
