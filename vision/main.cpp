#include <opencv2/opencv.hpp>
using namespace cv;

#include <iostream>
#include <cmath>
using namespace std;


int main(void)
{
    Mat frame = imread("test2.png");

    cvtColor(frame, frame, COLOR_BGR2GRAY);

    // Find the two circles.
    vector<Vec3f> circles;

    // Parameter values are very much obtained by trial and error
    HoughCircles(frame, // input
        circles, // output
        HOUGH_GRADIENT, // method
        1, // dp
        5, // minimum distance
        5, // param1
        24, // param2
        0, // minimum radius
        13); // maximum radius

    cout << "Found " << circles.size() << " circles." << endl;

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

    imshow("Hough Circles", frame);

    waitKey(0);
    return 0;
}
