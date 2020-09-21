#include "heda.h"
#include "position.h"
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

void detectCodes(Heda& heda, vector<DetectedHRCode>& detected, Mat& frame, PolarCoord c) {

  cout << "Running detect code." << endl;
  HRCodeParser parser(0.2, 0.2);
  vector<HRCode> positions;
  parser.findHRCodes(frame, positions, 100);

  if (!positions.empty()) {
    for (auto it = positions.begin(); it != positions.end(); ++it) {
      cout << "Detected one HRCode!!!" << endl;
      DetectedHRCode d(*it, c);
      detected.push_back(d);
    }
    return;
  }
  cout << "No codes were detected..." << endl;
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
  double epsilon = pow(HR_CODE_WIDTH*0.8, 2);
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
void CloseupCommand::setup(Heda& heda) {

  double robotZ = heda.config.user_coord_offset_z - detected.lid_coord.z;

  if (robotZ > heda.config.camera_radius) {
    robotZ = heda.config.camera_radius;
  }

  double userZ = heda.config.user_coord_offset_z - robotZ;
 
  commands.push_back(make_shared<HoverCommand>(detected.lid_coord.x, userZ, heda.config.camera_radius));

  commands.push_back(make_shared<MoveCommand>(heda.axisV, heda.unitV(detected.lid_coord.y + heda.config.closeup_distance)));

  commands.push_back(make_shared<LambdaCommand>([&](Heda& heda) {

    Mat frame;
    heda.captureFrame(frame);
    vector<DetectedHRCode> allDetected;
    detectCodes(heda, allDetected, frame, heda.getPosition());
    ensure(allDetected.size() >= 1, "There must be a detected code in a closup.");
    ensure(allDetected.size() <= 1, "There must be only one detected code in a closup.");
    
    DetectedHRCode recent = allDetected[0];

    detected.coord = recent.coord;
    detected.centerX = recent.centerX;
    detected.centerY = recent.centerY;
    detected.scale = recent.scale;
    detected.imgFilename = recent.imgFilename;

    pinpointCode(heda, detected);
    parseCode(heda, detected);
  
    heda.db.update(heda.codes, detected);
  }));
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

void parseCode(Heda& heda, DetectedHRCode& code) {
  //parseJarCode(code);
  cout << "Loading image: " << code.imgFilename << endl;
  Mat gray = imread(DETECTED_CODES_BASE_PATH + code.imgFilename, IMREAD_GRAYSCALE);
  vector<string> lines;
  parseText(lines, gray);
  ensure(lines.size() == 4, "There must be 4 lines in the HRCode.");
  code.jar_id = lines[0];
  code.weight = lines[1];
  code.content_name = lines[2];
  code.content_id = lines[3];
}

void ParseCodesCommand::start(Heda& heda) {
  for (size_t i = 0; i < heda.codes.items.size(); i++) {
    DetectedHRCode& code = heda.codes.items[i];
    parseCode(heda, code);
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

  int nStepX = 15;
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

      PolarCoord p = PolarCoord(x+deltaX, heda.unitV(heda.config.detect_height), t);
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
    //addMovementIfDifferent(movements, Movement('h', destination.h), position.h); 
    //addMovementIfDifferent(movements, Movement('t', destination.t), positionT); 
  } else {
    commands.push_back(make_shared<MoveCommand>(heda.axisT, destination.t)); 
    commands.push_back(make_shared<MoveCommand>(heda.axisH, destination.h)); 
    //addMovementIfDifferent(movements, Movement('t', destination.t), positionT); 
    //addMovementIfDifferent(movements, Movement('h', destination.h), position.h); 
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

Location getNewLocation(Heda& heda, Jar& jar) {
    
  JarFormatTable formats; heda.db.load(formats);
  JarFormat format;
  ensure(formats.get(format, jar.jar_format_id), "Existing jar must refer to an existing jar format");

  double widthNeeded = max(format.diameter, format.lid_diameter) + 32; // mm, FIXME: Hardcoded

  heda.shelves.order(byHeight);
  for (const Shelf& shelf : heda.shelves.items) {

    LocationTable locations;
    heda.db.load(locations, "WHERE shelf_id = " + to_string(shelf.id));

    for (double z = 0; z < shelf.depth - widthNeeded; z += 10) {
      for (double x = 0; x < shelf.width - widthNeeded; x += 10) {

        Vector2f wantedMin; wantedMin << x, z;
        Vector2f wantedMax; wantedMax << x+widthNeeded, z+widthNeeded;
        AlignedBox2f wanted(wantedMin, wantedMax);

        bool doesNotIntersect = true;
        for (const Location& loc : locations) {
        
          Vector2f locMin; locMin << loc.x-loc.diameter/2.0, loc.z-loc.diameter/2.0;
          Vector2f locMax; locMax << loc.x+loc.diameter/2.0, loc.z+loc.diameter/2.0;
          AlignedBox2f locBox(locMin, locMax);
          if (wanted.intersects(locBox)) {doesNotIntersect = false; break;}
        }

        if (doesNotIntersect) { // Good create new location
          Location loc;
          loc.x = x+widthNeeded/2.0;
          loc.z = z+widthNeeded/2.0;
          loc.diameter = widthNeeded;
          loc.shelf_id = shelf.id;
          loc.jar_format_id = format.id;
          heda.db.insert(locations, loc);
          return loc;
        }
      }
    }
  }

  return Location();
}

void StoreDetectedCommand::setup(Heda& heda) {
  // Do a closeup first
  commands.push_back(make_shared<CloseupCommand>(detected));
  //
  // Transform the detected code into a jar
  // Get or create a column if needed
  // Create a location
  // Move the jar to the location
 
  // TODO: How to get the updated code from the closeup???

  commands.push_back(make_shared<LambdaCommand>([&](Heda& heda) {
        
    ensure(detected.jar_id.size() == 3, "Jar id must have 3 digits");
    int id = atoi(detected.jar_id.c_str());

    ensure(heda.jars.find(jar, byJarId, id), "Detected jar id must refer to an existing jar");

    loc = getNewLocation(heda, jar); 
    ensure(loc.exists(), "Location could not be created. No space on shelves left? Can't save to database?");

    commands.push_back(make_shared<HoverCommand>(detected.lid_coord.x, detected.lid_coord.z, heda.config.gripper_radius));
    commands.push_back(make_shared<PickupCommand>(jar, loc));
    commands.push_back(make_shared<HoverCommand>(loc.x, loc.z, heda.config.gripper_radius));
    commands.push_back(make_shared<PutdownCommand>());
    commands.push_back(make_shared<HoverCommand>(loc.x, loc.z, heda.config.gripper_radius));
  }));
      
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
