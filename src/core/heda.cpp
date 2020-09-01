#include "heda.h"
#include "position.h"
#include "sweep.h"
#include "client_http.hpp"
#include "pinpoint.h"
#include "../lib/opencv.h"
//#include "calibrate.h"
#include <opencv2/highgui.hpp>
#include "jar_packer.h"

using HttpClient = SimpleWeb::Client<SimpleWeb::HTTP>;
using namespace std;

class InvalidJarIdException : public exception {};
class InvalidShelfException : public exception {};
class InvalidLocationException : public exception {};
class InvalidGrippedJarFormatException : public exception {};

void ensure(bool statement, const char* errorMessage) {
  if (!statement) {
    cerr << "\033[31mError\033[0m: " << errorMessage << endl;
    throw EnsureException();
  }
}

double detectedDistanceSquared(const DetectedHRCode& c1, const DetectedHRCode& c2) {
  Vector2f lid1; lid1 << c1.lid_coord.x, c1.lid_coord.z;
  Vector2f lid2; lid2 << c2.lid_coord.x, c2.lid_coord.z;
  return (lid1 - lid2).squaredNorm();
}

// Inefficient algorithm when n is large, but in my case n is small. O(n^2) I believe.
template <class T>
void removeNearDuplicates(vector<T> list, double (*func)(const T&, const T&), double epsilon) {
  for (typename vector<T>::iterator it = list.begin(); it != list.end(); it++) {
    for (auto nested : list) {
      if (it->id != nested.id && func(*it, nested) < epsilon) {
        list.erase(it--);
        goto next;
      }
    }
next:;
  }
}

void removeNearDuplicates(Heda& heda) {
  double epsilon = pow(HR_CODE_WIDTH/2, 2);
  //removeNearDuplicates(heda.codes, detectedDistanceSquared, epsilon);
  for (vector<DetectedHRCode>::iterator it = heda.codes.items.begin(); it != heda.codes.items.end(); it++) {
    for (auto nested : heda.codes.items) {
      if (it->id != nested.id && detectedDistanceSquared(*it, nested) < epsilon) {
        int id = it->id;
        it--;
        heda.db.removeItem(heda.codes, id);
        goto next;
      }
    }
next:;
  }
}

double computeFocalPoint(double perceivedWidth, double distanceFromCamera, double actualWidth) {

  return perceivedWidth * distanceFromCamera / actualWidth;
}

// closesup muste be done to the tallest jars first, then store them.
// then close up the next tallest, etc.
void closeup(Heda& heda) {
  heda.codes.order(byLidY, false);
  for (DetectedHRCode& code : heda.codes) {

    double z = code.lid_coord.z > heda.config.camera_radius ? heda.config.camera_radius : code.lid_coord.z;
    heda.pushCommand(make_shared<HoverCommand>(code.lid_coord.x, z, heda.config.camera_radius));

    heda.pushCommand(make_shared<MoveCommand>(heda.axisV, heda.unitV(code.lid_coord.y + 100))); // FIXME: Hardcoded CAMERA_CLOSUP_DISTANCE 

    heda.pushCommand(make_shared<LambdaCommand>([&code](Heda& heda) {
      Mat frame;
      heda.captureFrame(frame);
      vector<DetectedHRCode> detected;
      detectCodes(heda, detected, frame, heda.getPosition());
      ensure(detected.size() < 1, "There must be a detected code in a closup.");
      ensure(detected.size() > 1, "There must be only one detected code in a closup.");

      // Ugly, but I don't know how else to do this easily.
      const DetectedHRCode& recent = detected[0];
      code.coord = recent.coord;
      code.centerX = recent.centerX;
      code.centerY = recent.centerY;
      code.scale = recent.scale;
      code.imgFilename = recent.imgFilename;
      code.jar_id = recent.jar_id;
      code.weight = recent.weight;
      code.content_name = recent.content_name;
      code.content_id = recent.content_id;
      code.lid_coord = recent.lid_coord;

      heda.db.update(heda.codes, code);
    }));
  }
}
    
void HoverCommand::setup(Heda& heda) {
  Shelf shelf;
  ensure(heda.shelves.get(shelf, heda.shelfByHeight(heda.unitY(heda.m_position.v))), "hover must have a valid shelf to hover unto");

  UserCoord c(x, shelf.moving_height, z);
  commands.push_back(make_shared<GotoCommand>(heda.toPolarCoord(c, reference)));
}

// TODO: Check previous to make sure it improves and not doing an infinite loop.
/*void moveOver(Heda& heda, double previousDistanceSq) {

  pushCommand(make_shared<DetectCommand>());
  pushCommand(make_shared<PinpointCommand>());

  pushCommand(make_shared<LambdaCommand>([&format](Heda& heda) {

    if (heda.codes.items.size() < 1) {throw MissingHRCodeException();}
    if (heda.codes.items.size() > 1) {throw TooManyHRCodeException();}
    DetectedHRCode code = heda.codes.items[0];

    Vector2f detectedAt; detectedAt << code.centerX, code.centerY;
    Vector2f center; center << CAMERA_WIDTH / 2, CAMERA_HEIGHT / 2;
    double distSquare = (detectedAt - center).sqaredNorm();

    if (distSquare > previousDistanceSq) {
      cout << "The moveover distance has stopped improving. Dist = " << sqrt(distSquare) << " mm." << endl;
      // I think it should be camera to tool offset.
      cout << "Camera radius is = TODO" << endl;
      break;
    }
    cout << "The moveover distance is still getting better, continuing.." << endl;

    moveOver(heda, distSquare);
  }));
}*/

//void Heda::calibrate() {
//  // TODO: Validate that the working shelf is clear
//  // TODO: Make sure heda is referenced
//  // TODO: Use putdown function.
//
//  JarFormat format; 
//  if (!jar_formats.get(format, gripped_jar.jar_format_id)) {throw InvalidGrippedJarFormatException();}
//
//  //pushCommand(make_shared<GotoCommand>(PolarCoord(config.home_position_x, config.home_position_y, config.home_position_t)));
//  pushCommand(make_shared<StoreCommand>("a8"));
//  //pushCommand(make_shared<MoveCommand>(axisV, unitV(config.detect_height)));
//  pushCommand(make_shared<DetectCommand>());
//  pushCommand(make_shared<LambdaCommand>([&format](Heda& heda) {
//
//    if (heda.codes.items.size() < 1) {throw MissingHRCodeException();}
//    if (heda.codes.items.size() > 1) {throw TooManyHRCodeException();}
//    DetectedHRCode code = heda.codes.items[0];
//
//    double cameraDistance = heda.unitY(heda.getPosition().v) - format.height - heda.working_shelf.height;
//    //double codePixelsWidth = HR_CODE_WIDTH * code.scale;
//    heda.config.camera_focal_point = code.scale * cameraDistance;
//    //config.camera_focal_point = computeFocalPoint(codePixelsWidth, cameraDistance, HR_CODE_WIDTH);
//    heda.db.update(heda.configs, heda.config);
//  }));
//
//  // double jarHeight = gripped_jar.height;
//  //putdown(); 
//  //DetectedHRCode code = detectOneCode(Heda& heda);
//  //heda.camera_radius = ...
//
//}

void Heda::calibrate(JarFormat& format) {

  Mat frame;
  captureFrame(frame);
  vector<DetectedHRCode> detected;
  detectCodes(*this, detected, frame, getPosition());
 
  if (detected.size() < 1) {throw MissingHRCodeException();}
  if (detected.size() > 1) {throw TooManyHRCodeException();}

  double cameraDistance = unitY(getPosition().v) - format.height - working_shelf.height;
  config.camera_focal_point = detected[0].scale * cameraDistance;
  db.update(configs, config);
}

void ParseCodesCommand::start(Heda& heda) {
  for (size_t i = 0; i < heda.codes.items.size(); i++) {
    DetectedHRCode& code = heda.codes.items[i];
    parseJarCode(code);
    heda.db.update(heda.codes, code);
  }
}
    
void MetaCommand::start(Heda& heda) {
  setup(heda);
  currentCommand = commands.begin();
  (*currentCommand)->start(heda);
  heda.stack_writer << "Sub: Starting command: " + (*currentCommand)->str();
}

void OpenGripCommand::doneCallback(Heda& heda) {
  heda.gripped_jar.id = -1;
  heda.is_gripping = false;
}
    
/*PolarCoord toolCartesianToPolar(const CartesianCoord c) {

  double t = (asin(c(2) / CLAW_RADIUS) * 180.0 / PI);
  if (c(0) < X_MIDDLE) {
    t = 180 - t;
  }
  double deltaX = cosd(t) * CLAW_RADIUS;
  PolarCoord r;
  r << c(0)-deltaX, c(1), t;
  return r;
}*/
void SweepCommand::setup(Heda& heda) {

  heda.db.clear(heda.codes);
  
  PolarCoord max(heda.config.max_h, heda.config.max_v, 90.0);

  int nStepX = 9;
  int nStepZ = 5;

  bool zUp = true;
  for (int i = 0; i <= nStepX; i++) {
    for (int j = 0; j <= nStepZ; j++) {

      // These are the cartesian coordinates of the polar coordinate system. (Not UserCoord)
      double x = i*1.0*max.h/nStepX;
      double z = j*1.0*heda.config.gripper_radius/nStepZ;

      if (!zUp) {z = heda.config.gripper_radius - z;}

      // t is between -90 and 90. So to get between 180 and 90, if the arm if smaller than the middle, do t=180-t
      double t = (asin(z / heda.config.gripper_radius) * 180.0 / PI);
      if (x > max.h / 2) {
        t = 180.0 - t;
      }
      double deltaX = cosd(t) * heda.config.gripper_radius;

      PolarCoord p = PolarCoord(x+deltaX, heda.unitV(heda.working_shelf.moving_height), t);
      commands.push_back(make_shared<GotoCommand>(p));
      commands.push_back(make_shared<DetectCommand>());
    }
    zUp = !zUp;
  }

  commands.push_back(make_shared<PinpointCommand>());
  commands.push_back(make_shared<ParseCodesCommand>());
}

// Get lower, either to pickup, or to putdown
void LowerForGripCommand::setup(Heda& heda) {
  Shelf shelf;
  if (!heda.shelves.get(shelf, heda.shelfByHeight(heda.unitY(heda.m_position.v)))) {throw InvalidShelfException();}

  JarFormat format; 
  if (!heda.jar_formats.get(format, jar.jar_format_id)) {throw InvalidGrippedJarFormatException();}
 
  double v = heda.unitV(shelf.height + format.height + heda.config.grip_offset);
  commands.push_back(make_shared<MoveCommand>(heda.axisV, v));
}

void GripCommand::doneCallback(Heda& heda) {
  heda.gripped_jar = jar;
  heda.is_gripping = true;
}

void GripCommand::setup(Heda& heda) {
  commands.push_back(make_shared<GrabCommand>(40.0)); // TODO: Read the grab strength from the jar_format model
}

void PutdownCommand::setup(Heda& heda) {
  // TODO: Error message is not gripping
  if (heda.is_gripping) {
    commands.push_back(make_shared<LowerForGripCommand>(heda.gripped_jar));
    commands.push_back(make_shared<OpenGripCommand>());
  }
}

void GotoCommand::setup(Heda& heda) {

  PolarCoord position = heda.getPosition();

  int currentLevel = heda.shelfByHeight(heda.unitY(position.v));
  int destinationLevel = heda.shelfByHeight(heda.unitY(destination.v));
    
  double positionT = position.t;

  // must change level
  if (currentLevel != destinationLevel) {

    positionT = (position.h < X_MIDDLE) ? CHANGE_LEVEL_ANGLE_HIGH : CHANGE_LEVEL_ANGLE_LOW;
    commands.push_back(make_shared<MoveCommand>(heda.axisT, positionT));
  }

  commands.push_back(make_shared<MoveCommand>(heda.axisV, destination.v)); 
  //addMovementIfDifferent(movements, Movement('y', destination.v), position.v); 

  // If moving theta would colide, move x first
  double deltaH = cosd(destination.t) * heda.config.gripper_radius;
  double hIfTurnsFirst = position.h - deltaH;

  cout << "position.h: " << position.h << endl;
  cout << "deltaH: " << deltaH << endl;
  cout << "hIfTurnsFirst: " << hIfTurnsFirst << endl;
  cout << "destination.h: " << destination.h << endl;
  cout << "positionT: " << positionT << endl;

  if (hIfTurnsFirst < 0.0 || hIfTurnsFirst > heda.config.max_h) {

    // To avoid collision, if the arm goes over the middle, go to the middle before moving
    if ((positionT < 90.0 && destination.t > 90.0) || (positionT > 90.0 && destination.t < 90.0)) {
      commands.push_back(make_shared<MoveCommand>(heda.axisT, 90.0)); 
    }

    commands.push_back(make_shared<MoveCommand>(heda.axisH, destination.h)); 
    commands.push_back(make_shared<MoveCommand>(heda.axisT, destination.t)); 
    //addMovementIfDifferent(movements, Movement('x', destination.h), position.h); 
    //addMovementIfDifferent(movements, Movement('t', destination.t), positionT); 
  } else {
    commands.push_back(make_shared<MoveCommand>(heda.axisT, destination.t)); 
    commands.push_back(make_shared<MoveCommand>(heda.axisH, destination.h)); 
    //addMovementIfDifferent(movements, Movement('t', destination.t), positionT); 
    //addMovementIfDifferent(movements, Movement('x', destination.h), position.h); 
  }
}

bool MetaCommand::isDone(Heda& heda) {

  if (currentCommand == commands.end()) {return true;}

  if ((*currentCommand)->isDone(heda)) {
    heda.stack_writer << "Sub: Finished command: " + (*currentCommand)->str();
    (*currentCommand)->doneCallback(heda);
    currentCommand++;
    if (currentCommand == commands.end()) {return true;}
    (*currentCommand)->start(heda);
    heda.stack_writer << "Sub: Starting command: " + (*currentCommand)->str();
  }

  return false;
}

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
    

/*void Heda::fetch(std::string ingredientName) {
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
}*/

void PickupCommand::setup(Heda& heda) {
  NaiveJarPacker packer;
  commands.push_back(packer.moveToLocationCmd(heda, loc));
  commands.push_back(make_shared<LowerForGripCommand>(jar));
  commands.push_back(make_shared<GripCommand>(jar));
  commands.push_back(packer.moveToLocationCmd(heda, loc));
}

void PickupCommand::doneCallback(Heda& heda) {
  jar.location_id = -1;
  heda.db.update(heda.jars, jar);
}

void StoreCommand::setup(Heda& heda) {
  // TODO: Error message is not gripping
  if (heda.is_gripping) {

    NaiveJarPacker packer;
    if (!location_name.empty()) {
      if (heda.locations.byName(loc, location_name) < 0) {
        cout << "A location name was given, but it was not found. Aborting store.";
        return;
      }
    } else {
      int locId = packer.nextLocation(heda);
      heda.locations.get(loc, locId);
    }

    // TODO: Handle and show to the user all possible errors

    if (loc.exists()) {
      commands.push_back(packer.moveToLocationCmd(heda, loc));
      commands.push_back(make_shared<PutdownCommand>());
      commands.push_back(packer.moveToLocationCmd(heda, loc));
    }
  } else {
    cout << "Warning heda is not gripping. Aborting store..." << endl;
  }
}

void StoreCommand::doneCallback(Heda& heda) {
  if (loc.exists()) {
    heda.gripped_jar.location_id = loc.id;
    heda.db.update(heda.jars, heda.gripped_jar);
  }
}

void PinpointCommand::start(Heda& heda) {
  for (size_t i = 0; i < heda.codes.items.size(); i++) {
    DetectedHRCode& code = heda.codes.items[i];
    pinpointCode(heda, code);
    heda.db.update(heda.codes, code);
  }
}

void DetectCommand::start(Heda& heda) {
  Mat frame;
  heda.captureFrame(frame);
  vector<DetectedHRCode> detected;
  detectCodes(heda, detected, frame, heda.getPosition());
  for (DetectedHRCode& it : detected) {
    heda.db.insert(heda.codes, it);
  }
}

void Heda::capture() {
  Mat frame;
  captureFrame(frame);
  //thread show_image_thread([frame]() {
    imshow("capture", frame);
    waitKey(1000);
  //});
  //show_image_thread.detach();
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
