#include <iostream>

#include <opencv2/opencv.hpp>
#include <opencv2/core.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/highgui.hpp>
#include "lib/opencv.h"
#include "lib/hr_code.h"

#include "core/heda.h"
#include "core/pinpoint.h"

#include <Eigen/Core> // Really overkill to use eigen to calculate norm, at least use opencv since I have it
#include <Eigen/Geometry>

//void debugDetect(Heda& heda) {
// 
//  cv::Mat src = cv::imread("tmp/hrcode/finder/lastCapture.jpg", cv::IMREAD_COLOR);
//  vector<HRCode> positions;
//  findHRCodes(src, positions);
//}

//// Maybe use must as a convention for functions that can throw exception?
//DetectedHRCode mustDetectOneCode(Heda& heda) {
//
//  vector<DetectedHRCode> detected;
//  detectCodes(heda, detected);
//  ensure(detected.size() == 1, "There should have been one and only one code detected.");
//  return detected.back();
//}

using namespace std;

class Cluster {
  public:
    int idxKept;
    vector<int> indicesRemoved;
};

double distanceSquare(const UserCoord& c1, const UserCoord& c2) {
  return (c1.x-c2.x)*(c1.x-c2.x) + (c1.y-c2.y)*(c1.y-c2.y) + (c1.z-c2.z)*(c1.z-c2.z); 
}

double detectedDistanceSquared(const DetectedHRCode& c1, const DetectedHRCode& c2) {
  Eigen::Vector2f lid1; lid1 << c1.lid_x, c1.lid_z;
  Eigen::Vector2f lid2; lid2 << c2.lid_x, c2.lid_z;
  return (lid1 - lid2).squaredNorm();
}

// Inefficient algorithm when n is large, but in my case n is small. O(n^2) I believe.
void removeNearDuplicates(Heda& heda) {

  double epsilon = pow(HRCODE_OUTER_DIA * 2, 2); // FIXME: Be better than this, handle the jars closer together
  //removeNearDuplicates(heda.codes, detectedDistanceSquared, epsilon);
  vector<DetectedHRCode> codes = heda.db.all<DetectedHRCode>();
  vector<int> ids;
  vector<Cluster> clusters;
  for (unsigned int i = 0; i < codes.size(); i++) {

    DetectedHRCode& code = codes[i];
    // An item can only belong to one cluster. So discard if already belongs to one.
    if (count(ids.begin(), ids.end(), code.id) > 0) continue;

    Cluster cluster;
    cluster.idxKept = i;
    for (unsigned int j = i+1; j < codes.size(); j++) {
      DetectedHRCode& other = codes[j];
      if (detectedDistanceSquared(code, other) < epsilon) {
        ids.push_back(other.id);
        cluster.indicesRemoved.push_back(j);
      }
    }
    clusters.push_back(cluster);
  }
  for (Cluster& cluster : clusters) {

    double sumX = codes[cluster.idxKept].lid_x;
    double sumZ = codes[cluster.idxKept].lid_z;
    for (int &idx : cluster.indicesRemoved) {
      sumX += codes[idx].lid_x;
      sumZ += codes[idx].lid_z;
    }
    DetectedHRCode& code = codes[cluster.idxKept];
    code.lid_x = sumX / (cluster.indicesRemoved.size() + 1);
    code.lid_z = sumZ / (cluster.indicesRemoved.size() + 1);
    heda.db.update(code);
  }

  for (int &id : ids) {
    heda.db.deleteFrom<DetectedHRCode>("WHERE id = " + to_string(id));
  }
}

void parseCode(Heda& heda, DetectedHRCode& code) {
  //parseJarCode(code);
  cout << "Loading image: " << code.img << endl;
  cv::Mat gray = cv::imread(DETECTED_CODES_BASE_PATH + code.img, cv::IMREAD_GRAYSCALE);
  vector<string> lines;
  parseText(lines, gray);
  //ensure(lines.size() == 4, "There must be 4 lines in the HRCode.");
  code.jar_id = lines[0];
  //code.weight = lines[1];
  //code.content_name = lines[2];
  //code.content_id = lines[3];
}

void processCapture(Heda& heda, Capture& capture, std::vector<DetectedHRCode> &detected) {

  std::string filename = "../heda-recipes/" + capture.filename;
  std::cout << "Processing file: " << filename << std::endl;
  cv::Mat img = cv::imread(filename, cv::IMREAD_COLOR);

  if (img.empty()) {std::cerr << "Ignoring empty capture\n"; return;}

  vector<HRCode> positions;
  findHRCodes(img, positions);

  if (!positions.empty()) {
    for (auto it = positions.begin(); it != positions.end(); ++it) {
      cout << "Detected one HRCode!!!" << endl;
      DetectedHRCode d;
      d.h = capture.h;
      d.v = capture.v;
      d.t = capture.t;
      d.centerX = it->x;
      d.centerY = it->y;
      d.scale = it->scale;
      d.img = it->imgFilename;

      pinpointCode(heda, d);
      parseCode(heda, d);

      detected.push_back(d);
    }
  }
}

void detectAll(Heda& heda) {

  heda.db.clear<DetectedHRCode>();
  vector<DetectedHRCode> detected;

  std::vector<Capture> captures = heda.db.all<Capture>();
  for (Capture& capture : captures) {
    processCapture(heda, capture, detected);
  }
  
  for (DetectedHRCode& it : detected) {
    heda.db.insert(it);
  }

  removeNearDuplicates(heda);
}

int main(int argc, char** argv) {
  
  Database db("../heda-recipes/db/development.sqlite3"); // FIXME: HARDCODED
  Heda heda(db); 

  if (argc <= 1) {
    detectAll(heda);
  } else if (argc == 3) {

    int captureId = std::stoi(argv[1]);
    int detectedId = std::stoi(argv[2]);

    Capture capture = heda.db.find<Capture>(captureId);
    if (!capture.exists()) {return -4;}
    DetectedHRCode code = heda.db.find<DetectedHRCode>(detectedId);
    if (!code.exists()) {return -3;}

    vector<DetectedHRCode> detected;
    processCapture(heda, capture, detected);
    if (detected.size() != 1) {return -1;}

    DetectedHRCode updated = detected[0];
    updated.id = detectedId;
    heda.db.update(updated);
    
  } else {
    return -2;
  }

  return 0;
}

