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
class InvalidLocationException : public exception {};

void ReferencingCommand::doneCallback(Heda& heda) {
  if (axis.id == AXIS_H) {
    heda.m_position.h = REFERENCE_POSITION_H;
  } else if (axis.id == AXIS_V) {
    heda.m_position.v = REFERENCE_POSITION_V;
  } else if (axis.id == AXIS_T) {
    heda.m_position.t = REFERENCE_POSITION_T;
  //} else if (axis.id == AXIS_R) {
  }
  axis.is_referenced = true;
}

void MoveCommand::doneCallback(Heda& heda) {
  if (axis.id == AXIS_H) {
    heda.m_position.h = destination;
  } else if (axis.id == AXIS_V) {
    heda.m_position.v = destination;
  } else if (axis.id == AXIS_T) {
    heda.m_position.t = destination;
  //} else if (axis.id == AXIS_R) {
  }
  axis.is_referenced = true;
}

bool SlaveCommand::isDone(Heda& heda) {
  // Check if the message MESSAGE_DONE has been received.
  if (heda.m_reader.inputAvailable()) {

    string str = getInputLine(heda.m_reader);

    if (str == MESSAGE_DONE) { return true; }
  }
  return false;
}
    
void SlaveCommand::start(Heda& heda) {
  heda.m_writer << cmd;
}

void Heda::generateLocations() {
  NaiveJarPacker packer;
  packer.generateLocations(*this);
}
    
void Heda::pickup(Jar jar, const string& locationName) {
  NaiveJarPacker packer;
  Location loc;
  if (locations.byName(loc, locationName) < 0) {
    cout << "A location name was given, but it was not found. Aborting pickup.";
    return;
  }
  packer.moveToLocation(*this, loc);
  lowerForGrip(jar);
  grip(jar);
  jar.location_id = -1;
  db.update(jars, jar);
  packer.moveToLocation(*this, loc);
}

void Heda::fetch(std::string ingredientName) {
  cout << "About to fetch ingredient = " << ingredientName << endl;
  for (const Ingredient& ing : ingredients.items) {
    if (iequals(ing.name, ingredientName)) {

      cout << "The ingredient exists. Now checking to find a jar that has some." << endl;
      for (const Jar& jar : jars.items) {
        if (jar.ingredient_id == ing.id) {

          // TODO: Make sure the the quantity of ingredients left in the jar is ok.
          // Ok now get the location.
          Location loc;
          if (!locations.get(loc, jar.location_id)) {throw InvalidLocationException();} // TODO: Handle error message missing location
          NaiveJarPacker packer;
          packer.moveToLocation(*this, loc);
          // TODO: pickup(jar);
          return;
        }
      }
      cout << "Oups. No jar were found containing this ingredient..." << endl;
      return;
    }
  }
  cout << "Oups. The ingredient " << ingredientName << " was not found." << endl;
}

void Heda::store(const string& locationName) {
  // TODO: Error message is not gripping
  if (is_gripping) {

    NaiveJarPacker packer;
    Location loc;
    if (!locationName.empty()) {
      if (locations.byName(loc, locationName) < 0) {
        cout << "A location name was given, but it was not found. Aborting store.";
        return;
      }
    } else {
      int locId = packer.nextLocation(*this);
      locations.get(loc, locId);
    }

    // TODO: Handle and show to the user all possible errors

    if (loc.exists()) {
      packer.moveToLocation(*this, loc);
      putdown();
      gripped_jar.location_id = loc.id;
      db.update(jars, gripped_jar);
      packer.moveToLocation(*this, loc);
    }
  } else {
    cout << "Warning heda is not gripping. Aborting store..." << endl;
  }
}

void Heda::sweep() {
  std::cout << "Heda::sweep()\n";
  db.clear(codes);
  vector<Movement> mvts;
  calculateSweepMovements(*this, mvts);
  move(mvts);
}

void Heda::grip(Jar jar) {
  grab(40.0); // TODO: Read the grab strength from the jar_format model
  gripped_jar = jar; // TODO: Do this as a callback
  is_gripping = true;
}

void Heda::grip(int id) {
  Jar jar;
  if (!jars.get(jar, id)) {throw InvalidJarIdException();}
  grip(jar);
}

// Get lower, either to pickup, or to putdown
void Heda::lowerForGrip(const Jar& jar) {
  Shelf shelf;
  if (!shelves.get(shelf, shelfByHeight(toUserHeight(m_position)))) {throw InvalidShelfException();}
  PolarCoord p(m_position);

  JarFormat format; 
  if (!jar_formats.get(format, jar.jar_format_id)) {throw InvalidGrippedJarFormatException();}
  
  p.v = toPolarCoord(UserCoord(0.0, shelf.height + format.height + config.grip_offset, 0.0), config.gripper_radius).v;
  moveTo(p);
}

// It would be nice if the y axis was with an encoder... So just lower until there is a resistance.
// Lower to the shelf based on jar height
// Then release.
// Then back to it's previous height
void Heda::putdown() {
  // TODO: Error message is not gripping
  if (is_gripping) {

    lowerForGrip(gripped_jar);
    openJaw();
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
