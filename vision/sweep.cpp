#include <iostream>
#include <algorithm>
#include <vector>
#include <zbar.h>

#include <opencv2/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

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

  // disable all
  scanner.set_config(ZBAR_NONE, ZBAR_CFG_ENABLE, 0);

  // enable qr
  scanner.set_config(ZBAR_QRCODE, ZBAR_CFG_ENABLE, 1);
  
  // Convert image to grayscale
  Mat imGray;
  cvtColor(im, imGray,COLOR_BGR2GRAY);

  // Wrap image data in a zbar image
  Image image(im.cols, im.rows, "Y800", (uchar *)imGray.data, im.cols * im.rows);

  // Scan the image for barcodes and QRCodes
  int n = scanner.scan(image);
  
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

#define MAX_X 700.0
#define MAX_Z 380.0

void move(const char* txt, double pos) {
  cout << txt << pos << endl;
}

// x and z position is the position of the camera.
int main(int argc, char *argv[])
{
  double heights[] = {0.0};
  //double xSweepIntervals[] = {0, 100, 200, 300, 400, 500, 600, 700, '\0'};
  double zStep = MAX_Z / 2;
  double xStep = MAX_X / 4;

  bool xUp = true; // Wheter the x axis goes from 0 to MAX or from MAX to 0

  double x = 0;
  double z = 0;

  for (int i = 0; i < (sizeof(heights) / sizeof(double)); i++) {
    move("MZ",0);
    bool zUp = true; // Wheter the z axis goes from 0 to MAX or from MAX to 0

    move("MY",heights[i]);
    for (x = xUp ? 0 : MAX_X; xUp ? x <= MAX_X : x >= 0; x += xStep * (xUp ? 1 : -1)) {
      move("MX",x);
      for (z = zUp ? 0 : MAX_Z; zUp ? z <= MAX_Z : z >= 0; z += zStep * (zUp ? 1 : -1)) {
        move("MZ",z);
        // Detect new ones and move to them to get exact position.
        // findBarCodes();
        
      }
      zUp = !zUp;
    }
    xUp = !xUp;
  }

  return 0;
}


