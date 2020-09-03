#ifndef JAR_PARSER_H
#define JAR_PARSER_H

#include "model.h"

#include <tesseract/baseapi.h>
#include <leptonica/allheaders.h>

class TessParser {
  public:

    TessParser();

    std::string parseLine(const Mat& im);
    std::string parseDigitLine(const Mat& im);
    std::string parseDefault(const Mat& im);

    tesseract::TessBaseAPI ocr;
    tesseract::TessBaseAPI digitOcr;
    tesseract::TessBaseAPI defaultOcr;
};

string parseLineTesseract(const Mat& im);

#endif
