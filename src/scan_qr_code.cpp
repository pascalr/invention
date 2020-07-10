//Reference:https://www.learnopencv.com/opencv-qr-code-scanner-c-and-python/

#include <iostream>
#include <algorithm>
#include <vector>
#include <zbar.h>

#include <opencv2/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include "lib/opencv.h"

using namespace std;
using namespace cv;
using namespace zbar;

typedef struct {
  string type;
  string data;
  vector <Point> location;
} DecodedObject;

// Find and decode barcodes and QR codes
void decode(Mat &im, vector<DecodedObject>&decodedObjects)
{
  
  // Create zbar scanner
  ImageScanner scanner;

  scanner.set_config(ZBAR_NONE, ZBAR_CFG_ENABLE, 1);
  // disable all
  //scanner.set_config(ZBAR_NONE, ZBAR_CFG_ENABLE, 0);

  // enable qr
  //scanner.set_config(ZBAR_QRCODE, ZBAR_CFG_ENABLE, 1);
  
  // Convert image to grayscale
  Mat imGray;
  cvtColor(im, imGray,COLOR_BGR2GRAY);

  // Wrap image data in a zbar image
  Image image(im.cols, im.rows, "Y800", (uchar *)imGray.data, im.cols * im.rows);

  // Scan the image for barcodes and QRCodes
  int n = scanner.scan(image);

  cout << "About to print results" << endl;
  
  // Print results
  for(Image::SymbolIterator symbol = image.symbol_begin(); symbol != image.symbol_end(); ++symbol)
  {
    DecodedObject obj;
    
    obj.type = symbol->get_type_name();
    obj.data = symbol->get_data();
    
    // Print type and data
    cout << "Type : " << obj.type << endl;
    cout << "Data : " << obj.data << endl << endl;
    
    // Obtain location
    for(int i = 0; i< symbol->get_location_size(); i++)
    {
      obj.location.push_back(Point(symbol->get_location_x(i),symbol->get_location_y(i)));
    }
    
    decodedObjects.push_back(obj);
  }
}

int main(int argc, char *argv[])
{
  Mat im;
  // The first argument is an image path
  if ( argc == 2 ) {
    string imagepath = argv[1];
    im = imread(imagepath);
  } else { // Try to capture a video image
    captureVideoImage(im);
  }

  // Variable for decoded objects
  vector<DecodedObject> decodedObjects;

  // Find and decode barcodes and QR codes
  decode(im, decodedObjects);

  for(vector<DecodedObject>::iterator it = decodedObjects.begin(); it != decodedObjects.end(); ++it) {
    if (it->location.size() >= 4) {
      Point center = Point(0,0);
      center.x += it->location.at(0).x;
      center.x += it->location.at(1).x;
      center.x += it->location.at(2).x;
      center.x += it->location.at(3).x;
      center.y += it->location.at(0).y;
      center.y += it->location.at(1).y;
      center.y += it->location.at(2).y;
      center.y += it->location.at(3).y;
      // is there a function sum?
      center.x = center.x / 4;
      center.y = center.y / 4;
      circle(im, center, 5, Scalar(0, 0, 255), FILLED);
    }
    //for(vector<Point>::iterator it2 = it->location.begin(); it2 != it->location.end(); ++it2) {
    //  Point p = *it2;
    //  circle(im, p, 5, Scalar(0, 0, 255), FILLED);
    //}
  }
  //imshow("Live", im);
  imwrite("output.png", im);

  return 0;
}
