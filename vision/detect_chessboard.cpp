#include <opencv2/opencv.hpp>
#include <opencv2/core.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/highgui.hpp>
#include <iostream>
#include <stdio.h>

// The camera will be used to do a refencing.
// The pi will be directly connect to a arduino (safely watchout never 5v just 3v3).
// The rotating axis is referenced when it is farther than the middle.
// The vertical axis is referenced when it it higher than the middle (put another 8 by 8 grid on the other side would be the simplest)
// The horizontal axis is referenced when the squares are bigger than some threshold, offset for zero.

using namespace cv;
using namespace std;
int main(int, char**)
{

    vector<Point2f> corners; //this will be filled by the detected corners
    Size patternsize(7,7); //interior number of corners
    Mat gray;

    Mat frame;
    //--- INITIALIZE VIDEOCAPTURE
    VideoCapture cap;

    // open the default camera using default API
    // cap.open(0);
    // OR advance usage: select any API backend
    int deviceID = 0;             // 0 = open default camera
    int apiID = cv::CAP_ANY;      // 0 = autodetect default API
    // open selected camera using selected API
    cap.open(deviceID + apiID);
    // check if we succeeded
    if (!cap.isOpened()) {
        cerr << "ERROR! Unable to open camera\n";
        return -1;
    }
    //--- GRAB AND WRITE LOOP
    cout << "Start grabbing" << endl
        << "Press any key to terminate" << endl;
    for (;;)
    {
        // wait for a new frame from camera and store it into 'frame'
        cap.read(frame);
        // check if we succeeded
        if (frame.empty()) {
            cerr << "ERROR! blank frame grabbed\n";
            break;
        }

        cvtColor(frame, gray, COLOR_BGR2GRAY);
        bool patternfound = findChessboardCorners(gray, patternsize, corners,
          CALIB_CB_ADAPTIVE_THRESH + CALIB_CB_NORMALIZE_IMAGE + CALIB_CB_FAST_CHECK);

        if(patternfound) {
          cornerSubPix(gray, corners, Size(11, 11), Size(-1, -1), TermCriteria(2, 30, 0.1));
          //cornerSubPix(gray, corners, Size(11, 11), Size(-1, -1),
           // TermCriteria(CV_TERMCRIT_EPS + CV_TERMCRIT_ITER, 30, 0.1));
         
          drawChessboardCorners(frame, patternsize, Mat(corners), patternfound);
	}

        // show live and wait for a key with timeout long enough to show images
        imshow("Live", frame);
        if (waitKey(5) >= 0)
            break;
    }
    // the camera will be deinitialized automatically in VideoCapture destructor
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
