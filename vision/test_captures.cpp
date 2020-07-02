#include <opencv2/opencv.hpp>
#include <opencv2/core.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/highgui.hpp>
#include <iostream>
#include <stdio.h>
#include <string>
#include <chrono>

#include "common.h"

#include "dirent.h"

#include "hr_code.hpp"

//#include <filesystem>
//namespace fs = std::filesystem;
//fs::exists("capture.cpp");

using namespace cv;
using namespace std;

#include <boost/log/core.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/expressions.hpp>

namespace logging = boost::log;

template <class P>
void assertTest(const char* info, P t1, P t2) {
  cout << (t1 == t2 ? "\033[32mPASSED\033[0m" : "\033[31mFAILED\033[0m");
  cout << " (" << info << ")";
  cout << " - Expected: " << t1 << ", " << "Got: " << t2 << endl;
}

int main(int argc, char** argv)
{
  logging::core::get()->set_filter(
    logging::trivial::severity >= logging::trivial::info
  );

  DIR *dir;
  struct dirent *ent;
  if ((dir = opendir ("output/")) != NULL) {
    /* print all the files and directories within directory */
    while ((ent = readdir (dir)) != NULL) {
      if (startsWith("capture",ent->d_name)) {
        string filename = "output/"; filename += ent->d_name;
        Mat mat = imread( filename );
        vector<HRCode> codes = detectHRCodes(mat);
        if (!codes.empty()) {
          assertTest(filename.c_str(), codes[0].isValid(), true);
        } else {
          assertTest((filename + ": No codes detected").c_str(), true, false);
        }
      }
    }
    closedir (dir);
  } else {
    perror ("could not open directory");
    return EXIT_FAILURE;
  }

  return 0;
}


