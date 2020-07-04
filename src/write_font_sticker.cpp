/*#include <opencv2/opencv.hpp>
#include <opencv2/core.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/freetype.hpp>
#include <iostream>
#include <stdio.h>

#include "common.h"

using namespace cv;
using namespace std;

int main(int, char**)
{
  string jarIdTxt = "#1";
  string jarWeightTxt = "0.095g";
  string jarContentName = "Moutarde";
  string jarContentId = "880";

  string text = "Funny text inside the box";
  int fontHeight = 60;
  int thickness = -1;
  int linestyle = 8;
  Mat img(600, 800, CV_8UC3, Scalar::all(0));
  int baseline=0;
  cv::Ptr<cv::freetype::FreeType2> ft2;
  ft2 = cv::freetype::createFreeType2();
  ft2->loadFontData( "./mplus-1p-regular.ttf", 0 );
  Size textSize = ft2->getTextSize(text,
                                   fontHeight,
                                   thickness,
                                   &baseline);
  if(thickness > 0){
      baseline += thickness;
  }
  // center the text
  Point textOrg((img.cols - textSize.width) / 2,
                (img.rows + textSize.height) / 2);
  // draw the box
  rectangle(img, textOrg + Point(0, baseline),
            textOrg + Point(textSize.width, -textSize.height),
            Scalar(0,255,0),1,8);
  // ... and the baseline first
  line(img, textOrg + Point(0, thickness),
       textOrg + Point(textSize.width, thickness),
       Scalar(0, 0, 255),1,8);
  // then put the text itself
  ft2->putText(img, text, textOrg, fontHeight,
               Scalar::all(255), thickness, linestyle, true );

  int fontFace = FONT_HERSHEY_SCRIPT_SIMPLEX;
  double fontScale = 2;
  int thickness = 3;
  
  Mat img(600, 800, CV_8UC3, Scalar::all(0));
  
  int baseline=0;
  Size textSize = getTextSize(text, fontFace,
                              fontScale, thickness, &baseline);
  baseline += thickness;
  
  // center the text
  Point textOrg((img.cols - textSize.width)/2,
                (img.rows + textSize.height)/2);
  
  // draw the box
  rectangle(img, textOrg + Point(0, baseline),
            textOrg + Point(textSize.width, -textSize.height),
            Scalar(0,0,255));
  // ... and the baseline first
  line(img, textOrg + Point(0, thickness),
       textOrg + Point(textSize.width, thickness),
       Scalar(0, 0, 255));
  
  // then put the text itself
  putText(img, text, textOrg, fontFace, fontScale,
        Scalar::all(255), thickness, 8);
  
  showImgAndWaitForKey(img);

  return 0;
}*/

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
