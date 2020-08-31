#include <iostream>

#include "heda.h"
#include "writer/serial_writer.h"
#include "reader/serial_reader.h"

using namespace std;

class MissingHRCodeException : public exception {};
class TooManyHRCodeException : public exception {};

// https://www.pyimagesearch.com/2015/01/19/find-distance-camera-objectmarker-using-python-opencv/
// F = (P x  D) / W
// The focal point (F) equals the apparent width in pixels (P) times the distance from the camera D
// divided by the actual width of the object (W)
// I find F by an average with some data.
// Then I can always get the distance from the other values.
// D’ = (W x F) / P

// The focal point can be used to determine the distance of an object.

