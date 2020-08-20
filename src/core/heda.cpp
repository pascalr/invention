#include "heda.h"
#include "position.h"
#include "sweep.h"
#include "client_http.hpp"
#include "pinpoint.h"
#include "../lib/opencv.h"
#include "calibrate.h"
#include <opencv2/highgui.hpp>
#include "jar_packer.h"

using HttpClient = SimpleWeb::Client<SimpleWeb::HTTP>;

class InvalidJarIdException : public exception {};
class InvalidShelfException : public exception {};
    
void Heda::generateLocations() {
  NaiveJarPacker packer;
  packer.generateLocations(*this);
}

void Heda::store() {
  // TODO: Error message is not gripping
  if (is_gripping) {
    NaiveJarPacker packer;
    int locId = packer.nextLocation(*this);
    Location loc;
    if (locations.get(loc, locId)) {
      packer.moveToLocation(*this, loc);
      putdown();
      gripped_jar.location_id = loc.id;
      db.update(jars, gripped_jar);
    }
  }
}

void Heda::sweep() {
  std::cout << "Heda::sweep()\n";
  db.clear(codes);
  vector<Movement> mvts;
  calculateSweepMovements(*this, mvts);
  move(mvts);
}

void Heda::grip(int id) {
  Jar jar;
  if (!jars.get(jar, id)) {throw InvalidJarIdException();}
  
  grab(40.0); // TODO: Read the grab strength from the jar_format model
  gripped_jar = jar; // TODO: Do this as a callbacké
  is_gripping = true;
}

// It would be nice if the y axis was with an encoder... So just lower until there is a resistance.
// Lower to the shelf based on jar height
// Then release.
// Then back to it's previous height
void Heda::putdown() {
  // TODO: Error message is not gripping
  if (is_gripping) {

    double previousHeight = m_position.y;

    Shelf shelf;
    if (!shelves.get(shelf, shelfByHeight(toUserHeight(m_position)))) {throw InvalidShelfException();}
    PolarCoord p(m_position);

    JarFormat format; 
    if (!jar_formats.get(format, gripped_jar.jar_format_id)) {throw InvalidGrippedJarFormatException();}
  
    p.y = toPolarCoord(UserCoord(0.0, shelf.height + format.height + 10, 0.0), config.gripper_radius).y; // FIXME HARDCODED VALUE 7
    moveTo(p);
    openJaw();
    p.y = previousHeight;
    moveTo(p);
  }
}

void Heda::pinpoint() {
  for (size_t i = 0; i < codes.items.size(); i++) {
    DetectedHRCode& code = codes.items[i];
    pinpointCode(*this, code);
    db.update(codes, code);
  }
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

void Heda::capture() {
  Mat frame;
  captureFrame(frame);
  imshow("capture", frame);
  waitKey();
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
