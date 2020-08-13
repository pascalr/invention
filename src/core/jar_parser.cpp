#include <tesseract/baseapi.h>
#include <leptonica/allheaders.h>

#include "Model.h"
#include "../lib/hr_code.h"
#include <string>

/*class JarLabel {
  public:
    JarLabel() {}

    string getJarIdStr() {
      return m_jar_id;
    }

    int getJarId() {
      return strtol(m_jar_id.c_str(), &p, 10);
    }

    bool isJarIdValid(string& err = "") {
      if (m_jar_id.length() != 3) {
        err += "Error jar id length was " << m_jar_id.length() << " expected 3.\n";
        return false;
      }
      char* p;
      strtol(m_jar_id.c_str(), &p, 10);
      if (*p != 0) {
        err += "Error jar id bad conversion to number.\n";
        return false;
      }
      return true;
    }

    void setJarId(const string& s) {
      m_jar_id.assign(s);
      trim(m_jar_id);
    }

    string getWeightStr() {
      return m_weight;
    }

    float getWeight() {
      char *p;
      return strtof(m_weight.substr(0,5).c_str(), &p, 10);
    }

    bool isWeightValid(string& err = "") {
      if (m_weight.length() != 7) {
        err += "Error weight length was " << m_weight.length() << " expected 7.\n";
        return false;
      }
      char* p;
      strtof(m_weight.substr(0,5).c_str(), &p, 10);
      if (*p != 0) {
        err += "Error weight bad conversion to number.\n";
        return false;
      }
      return true;
    }

    void setWeight(const string& s) {
      m_weight.assign(s);
      trim(m_weight);
    }

    string getContentName() {
      // Maybe latter make sure that the content id matches the name
      // but for now it doesn't.
      return m_content_name;
    }

    bool isContentNameValid(string& err = "") {
      return true;
    }

    void setContentName(const string& s) {
      m_content_name.assign(s);
      trim(m_content_name);
    }

    string getContentIdStr() {
      return m_content_id;
    }

    int getContentId() {
      char *p;
      return strtol(m_content_id.c_str(), &p, 10);
    }

    bool isContentIdValid(string& err = "") {
      if (m_content_id.length() != 4) {
        err += "Error content id length was " << m_jar_id.length() << " expected 4.\n";
        return false;
      }
      char* p;
      strtol(m_content_id.c_str(), &p, 10);
      if (*p != 0) {
        err += "Error content id bad conversion to number.\n";
        return false;
      }
      return true;
    }

    void setContentId(const string& s) {
      m_content_id.assign(s);
      trim(m_content_id);
    }

    friend ostream &operator<<(std::ostream &os, const HRCode &m);
    
    bool isValid() {
      return isJarIdValid() && isWeightValid() && isNameValid() && isContentIdValid();
    }
    
    double x;
    double y;
    double z;

  private:

    string m_jar_id;
    string m_weight;
    string m_content_name;
    string m_content_id;
};

ostream &operator<<(std::ostream &os, const HRCode &m) {
  os << "#" << m.getJarIdStr() << "[" << m.getWeightStr() << " kg]: ";
  return os << m.getContentName() << " (" << m.mContentIdStr() << ")";
}*/

string parseLineTesseract(Mat& im) {
  tesseract::TessBaseAPI *ocr = new tesseract::TessBaseAPI();
  char config_name[] = "chars";
  char* config_ptr = config_name;
  //char config[][10] = {"chars"};
  //ocr->Init("tessdata", "eng", tesseract::OEM_LSTM_ONLY, config, 1, nullptr, nullptr, false);
  ocr->Init("tessdata", "eng", tesseract::OEM_LSTM_ONLY, &config_ptr, 1, nullptr, nullptr, false);
  ocr->SetVariable("user_defined_dpi", "300"); // FIXME: Is it
  //ocr->SetVariable("user_words_suffix", "eng.user-words"); Does this work? Rebuild and retrain my own dictionnary I think with only words that can be content.
  // But that means everytime a user adds a new product, it must retrain et rebuild everything??? Maybe, if it's fast to do...
  ocr->SetPageSegMode(tesseract::PSM_SINGLE_LINE);
  // ocr->SetPageSegMode(tesseract::PSM_SINGLE_WORD);

  ocr->SetImage(im.data, im.cols, im.rows, 3, im.step);
  string outText = string(ocr->GetUTF8Text());
  trim(outText);

  BOOST_LOG_TRIVIAL(debug) << outText;
  ocr->End();

  return outText;
}

void parseJarCode(DetectedHRCode& p) {
  Mat gray = imread(p.imgFilename, IMREAD_GRAYSCALE);
  BOOST_LOG_TRIVIAL(debug) << "Mat cols: " << gray.cols;
  double scale = gray.cols/110.0;
  BOOST_LOG_TRIVIAL(debug) << "scale: " << scale;
  double topOffset = 10.0 * scale;
  double charWidth = 12 * scale;
  double charHeight = 26 * scale;
  double lineInterspace = 24 * scale;

  int nbLines = 4;
  int pattern[nbLines] = {3,7,8,4}; // number of char per line
  string rawHRCode[nbLines];

  // Get the sub-matrices (minors) for every character.
  for (int i = 0; i < nbLines; i++) {
    int nbChar = pattern[i];
    double y = i*lineInterspace + topOffset;
    /*for (int j = 0; j < nbChar; j++) {
      double x = (0.0+j-1.0*nbChar/2.0)*charWidth + mat.cols/2;
      Rect r = Rect(x, y, charWidth, charHeight);
      //rectangle(mat, r, Scalar(0,0,255), 1, LINE_8);
      //Mat charMat(mat, Rect(x, y, charWidth, charHeight));
    }*/
    double x = nbChar/-2.0*charWidth + gray.cols/2;
    Rect lineRect = Rect(x, y, nbChar*charWidth, charHeight);
    //rectangle(mat, lineRect, Scalar(0,255,0), 1, LINE_8);
    Mat lineMat(gray, lineRect);
    //imshow(string("line")+to_string(i),lineMat);
    rawHRCode[i] = parseLineTesseract(lineMat);
  }

  /*HRCode code;
  code.setJarId(rawHRCode[0]);
  code.setWeight(rawHRCode[1]);
  code.setName(rawHRCode[2]);
  code.setContentId(rawHRCode[3]);
  if (code.isValid()) {
    BOOST_LOG_TRIVIAL(info) << "Detected code: " << code;
  }*/
}
