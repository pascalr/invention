#include "heda.h"
#include "client_http.hpp"
#include "../lib/opencv.h"
#include <opencv2/highgui.hpp>

using HttpClient = SimpleWeb::Client<SimpleWeb::HTTP>;
using namespace std;

//Header1* CURRENT_HEADER_1 = NULL;
//
//std::string currentHeaders() {
//}

void Heda::captureFrame(Mat& frame) {

  HttpClient client("192.168.0.19:8889");
  try {
    auto r1 = client.request("GET", "/slow_capture.jpg");

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
  } catch(const SimpleWeb::system_error &e) { // e.what()
    ensure(false, "Unable to capture frame.");
  }
  if (frame.empty()) {
    ensure(false, "Was able to capture frame, but it was empty.");
  }
}
