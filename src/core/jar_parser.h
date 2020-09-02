#ifndef JAR_PARSER_H
#define JAR_PARSER_H

#include "model.h"

void parseJarCode(DetectedHRCode& p);
string parseCharTesseract(const Mat& im);
string parseLineTesseract(const Mat& im);

#endif
