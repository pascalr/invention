#include "Heda.h"
#include "position.h"
#include "sweep.h"
#include "client_http.hpp"
#include "../lib/opencv.h"

using HttpClient = SimpleWeb::Client<SimpleWeb::HTTP>;

void Heda::sweep() {
  std::cout << "Heda::sweep()\n";
  db.clear(codes);
  vector<Movement> mvts;
  calculateSweepMovements(*this, mvts);
  move(mvts);
}

void Heda::parse() {
  for (size_t i = 0; i < codes.items.size(); i++) {
    DetectedHRCode& code = codes.items[i];
    parseJarCode(code);
    db.update(codes, code);
  }
}

void Heda::detect() {
  Mat frame;
  captureFrame(frame);
  detectCode(*this, frame, getPosition());
}

void Heda::captureFrame(Mat& frame) {

  HttpClient client("192.168.0.19:8889");
  try {
    auto r1 = client.request("GET", "/capture.jpg");

    //vector<uchar> v(131072); I can't do push_back with this...
    vector<uchar> v;
    while (true) {
      char ch = r1->content.rdbuf()->sgetc();
      if (r1->content.rdbuf()->snextc() == std::streambuf::traits_type::eof()) {break;}
      v.push_back(ch);
    }

    //vector<uchar> decodeBuf(r1->content.rdbuf()->begin(), r1->content.rdbuf()->end());
    //r1->content.rdbuf()->sgetn(decodeBuf, r1->content.size());
    frame = imdecode(Mat(v), IMREAD_COLOR);
  } catch(const SimpleWeb::system_error &e) {
    cerr << "Client request error: " << e.what() << endl;
    throw FrameCaptureException();
  }
  if (frame.empty()) {
    throw FrameCaptureException();
  }
}
