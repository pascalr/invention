#include <tesseract/baseapi.h>
#include <leptonica/allheaders.h>

#include <opencv2/imgcodecs.hpp>
#include "../lib/opencv.h"
#include "model.h"
#include "../lib/hr_code.h"
#include <string>
#include "../utils/constants.h"

// These files are only included in order to debug, to manipulate the image
#include <opencv2/opencv.hpp>
#include <opencv2/core.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/highgui.hpp>

#include "tess_parser.h"
    
TessParser::TessParser() {
  ocr.Init("tessdata", "eng", tesseract::OEM_LSTM_ONLY);
  ensure(ocr.SetVariable("load_system_dawg", "false"), "load_system_dawg");
  ensure(ocr.SetVariable("load_freq_dawg", "false"), "load_freq_dawg");
  ensure(ocr.SetVariable("user_defined_dpi", "300"), "user_defined_dpi");
  ensure(ocr.SetVariable("tessedit_write_images", "true"), "tessedit_write_images");
  ocr.SetPageSegMode(tesseract::PSM_SINGLE_LINE);
  
  digitOcr.Init("tessdata", "eng", tesseract::OEM_LSTM_ONLY);
  ensure(digitOcr.SetVariable("tessedit_char_whitelist", "0123456789"), "tessedit_char_whitelist");
  ensure(digitOcr.SetVariable("load_system_dawg", "false"), "load_system_dawg");
  ensure(digitOcr.SetVariable("load_freq_dawg", "false"), "load_freq_dawg");
  ensure(digitOcr.SetVariable("user_defined_dpi", "300"), "user_defined_dpi");
  ensure(digitOcr.SetVariable("tessedit_write_images", "true"), "tessedit_write_images");
  digitOcr.SetPageSegMode(tesseract::PSM_SINGLE_LINE);
}

std::string TessParser::parseLine(const Mat& im) {
  ocr.SetImage(im.data, im.cols, im.rows, 1, im.step);
  string outText = string(ocr.GetUTF8Text());
  trim(outText);
  return outText;
}
std::string TessParser::parseDigitLine(const Mat& im) {
  digitOcr.SetImage(im.data, im.cols, im.rows, 1, im.step);
  string outText = string(digitOcr.GetUTF8Text());
  trim(outText);
  return outText;
}

string parseLineTesseract(const Mat& im) {
  tesseract::TessBaseAPI *ocr = new tesseract::TessBaseAPI();
  ocr->Init("tessdata", "eng", tesseract::OEM_LSTM_ONLY);
  ocr->SetVariable("tessedit_char_whitelist", "0123456789");
  ocr->SetVariable("load_system_dawg", "false");
  ocr->SetVariable("load_freq_dawg", "false");
  ocr->SetVariable("tessedit_write_images", "true");
  ocr->SetPageSegMode(tesseract::PSM_SINGLE_LINE);
  ocr->SetImage(im.data, im.cols, im.rows, 3, im.step);
  string outText = string(ocr->GetUTF8Text());
  trim(outText);

  ocr->End();

  return outText;
}

/*string parseLineTesseract(const Mat& im) {
  tesseract::TessBaseAPI *ocr = new tesseract::TessBaseAPI();
  char config_name[] = "chars";
  char* config_ptr = config_name;
  //char config[][10] = {"chars"};
  //ocr->Init("tessdata", "eng", tesseract::OEM_LSTM_ONLY, config, 1, nullptr, nullptr, false);
  ocr->Init("tessdata", "eng", tesseract::OEM_LSTM_ONLY, &config_ptr, 1, nullptr, nullptr, false);
  ocr->SetPageSegMode(tesseract::PSM_SINGLE_LINE);
  // ocr->SetPageSegMode(tesseract::PSM_SINGLE_WORD);
  ocr->SetVariable("user_defined_dpi", "300"); // FIXME: Is it
  //ocr->SetVariable("user_words_suffix", "eng.user-words"); Does this work? Rebuild and retrain my own dictionnary I think with only words that can be content.
  // But that means everytime a user adds a new product, it must retrain et rebuild everything??? Maybe, if it's fast to do...

  ocr->SetImage(im.data, im.cols, im.rows, 3, im.step);
  string outText = string(ocr->GetUTF8Text());
  trim(outText);

  ocr->End();

  return outText;
}*/
