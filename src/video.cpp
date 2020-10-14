#include <opencv2/core.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/highgui.hpp>
#include <iostream>
#include <stdio.h>

using namespace cv;
using namespace std;

int main(int, char**)
{
  Mat frame;
  VideoCapture cap;
  int deviceID = 0;     // 0 = open default camera
  int apiID = cv::CAP_ANY;    // 0 = autodetect default API
  //int apiID = cv::CAP_V4L2;
  cap.open(deviceID, apiID);
  // 640 x 480
  // 800 x 600
  // 1280 x 720
  // 1280 x 800
  // 1600 x 1200
  // 1920 x 1080
  // 2048 x 1536
  // 2592 x 1944
  // 3264 x 2448
  cap.set(cv::CAP_PROP_FRAME_WIDTH, 1600);
  cap.set(cv::CAP_PROP_FRAME_HEIGHT, 1200);
  if (!cap.isOpened()) {
    cerr << "ERROR! Unable to open camera\n";
    return -1;
  }
  // https://www.arducam.com/product/b0196arducam-8mp-1080p-usb-camera-module-1-4-cmos-imx219-mini-uvc-usb2-0-webcam-board-with-1-64ft-0-5m-usb-cable-for-windows-linux-android-and-mac-os/
  // Adjustable Features: Brightness, Contrast, Saturation, Sharpness, Gamma, White balance.
  //And a lot more... https://docs.opencv.org/3.4/d4/d15/group__videoio__flags__base.html#gaeb8dd9c89c10a5c63c139bf7c4f5704d
  cout << "Camera opened ("<< cap.get(CAP_PROP_FRAME_WIDTH) << "x" << cap.get(CAP_PROP_FRAME_HEIGHT) << ")\n";
  cout << "Brightness: " << cap.get(CAP_PROP_BRIGHTNESS) << endl;
  cout << "Contrast: " << cap.get(CAP_PROP_CONTRAST) << endl;
  cout << "Saturation: " << cap.get(CAP_PROP_SATURATION) << endl;
  cout << "Sharpness: " << cap.get(CAP_PROP_SHARPNESS) << endl;
  cout << "Gamma: " << cap.get(CAP_PROP_GAMMA) << endl;
  cout << "Exposure: " << cap.get(CAP_PROP_EXPOSURE) << endl;
  cout << "Auto exposure: " << cap.get(CAP_PROP_AUTO_EXPOSURE) << endl;
  // White balance???
  cout << "Hue: " << cap.get(CAP_PROP_HUE) << endl;
  cout << "Gain: " << cap.get(CAP_PROP_GAIN) << endl;
  for (;;) {
    cap.read(frame);
    if (frame.empty()) {
      cerr << "ERROR! blank frame grabbed\n";
      break;
    }
    imshow("Live", frame);
    if (waitKey(5) >= 0)
      break;
  }
  return 0;
}
