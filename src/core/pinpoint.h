#ifndef JAR_POSITION_DETECTOR
#define JAR_POSITION_DETECTOR

// Maybe rename to pinpoint

#include "position.h"
#include <vector>
#include "model.h"
#include "heda.h"
 
using namespace std;
using namespace Eigen;


// https://www.pyimagesearch.com/2015/01/19/find-distance-camera-objectmarker-using-python-opencv/
// F = (P x  D) / W
// The focal point (F) equals the apparent width in pixels (P) times the distance from the camera D
// divided by the actual width of the object (W)
// I find F by an average with some data.
// Then I can always get the distance from the other values.
// D’ = (W x F) / P

double heightOffset(Heda& heda, DetectedHRCode& input);

Vector2d imageOffset(DetectedHRCode& input);

void pinpointCode(Heda& heda, DetectedHRCode& input);

#endif
