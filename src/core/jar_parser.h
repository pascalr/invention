#ifndef JAR_PARSER_H
#define JAR_PARSER_H

class JarLabel {
  public:
    HRCode() {}

    void setJarId(string s) {
      trim(s);
      if (s.length() != 3) {
        BOOST_LOG_TRIVIAL(debug) << "Error jar id length was " << s.length() << " expected 3.";
        return;
      }
      char* p;
      m_jar_id = strtol(s.c_str(), &p, 10);
      if (*p == 0) {
        m_jar_id_valid = true;
      } else {
        logError("Error jar id bad conversion to number.");
      }
    }

    void setWeight(string s) {
      trim(s);
      if (s.length() != 7) {
        BOOST_LOG_TRIVIAL(debug) << "Error weight length was " << s.length() << " expected 7.";
        return;
      }
      char* p;
      m_weight = strtof(s.substr(0,5).c_str(), &p);
      if (*p == 0) {
        m_weight_valid = true;
      } else {
        logError("Error weight bad conversion to number.");
      }
    }

    // Maybe latter make sure that the content id matches the name
    // but for now it doesn't.
    void setName(string s) {
      trim(s);
      m_name = s;
    }

    void setContentId(string s) {
      trim(s);
      if (s.length() != 4 && s.length() != 3) {
        BOOST_LOG_TRIVIAL(debug) << "Error content id length was " << s.length() << " expected 3 or 4.";
        return;
      }
      char* p;
      m_content_id = strtol(s.c_str(), &p, 10);
      if (*p == 0) {
        m_content_id_valid = true;
      } else {
        logError("Error contentId conversion to number.");
      }
    }

    friend ostream &operator<<(std::ostream &os, const HRCode &m);
    
    bool isValid() {
      return m_jar_id_valid && m_weight_valid && m_name_valid && m_content_id_valid;
    }
    
    double centerX;
    double centerY;

  private:

    bool logError(const char* msg) {
      BOOST_LOG_TRIVIAL(info) << msg;
      return false;
    }

    bool m_jar_id_valid = false;
    bool m_weight_valid = false;
    bool m_name_valid = true;
    bool m_content_id_valid = false;
    int m_jar_id = -1;
    float m_weight = 0;
    string m_name;
    int m_content_id;
};

ostream &operator<<(std::ostream &os, const HRCode &m) {
  os << "#" << m.m_jar_id << "[" << m.m_weight << " kg]: ";
  return os << m.m_name << " (" << m.m_content_id << ")";
}

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

HRCode parseHRCode(HRCodePosition& p) {
  Mat gray;
  cvtColor(p.img, gray, COLOR_GRAY2BGR );
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

  HRCode code;
  code.setJarId(rawHRCode[0]);
  code.setWeight(rawHRCode[1]);
  code.setName(rawHRCode[2]);
  code.setContentId(rawHRCode[3]);
  code.centerX = p.x;
  code.centerY = p.y;
  if (code.isValid()) {
    BOOST_LOG_TRIVIAL(info) << "Detected code: " << code;
  }

  return code;
}

vector<HRCode> detectHRCodes(Mat& src) {

  HRCodeParser parser(0.2, 0.2);
  vector<HRCodePosition> positions;
  parser.findHRCodePositions(src, positions, 100);

  vector<HRCode> codes(positions.size());
  for(size_t i = 0; i < positions.size(); i++) {
    codes[i] = parseHRCode(positions[i]);
  }

  return codes;
}

#endif
