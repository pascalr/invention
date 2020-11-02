#ifndef _HEDA_CONTROLLER_H
#define _HEDA_CONTROLLER_H

#include "../common/comm.h"
#include "pinpoint.h"
#include "recipe_parser.h"
#include "heda.h"
#include <thread>
#include <chrono>
#include <iostream>
#include "../lib/opencv.h"
#include <opencv2/highgui.hpp>
#include "log.h"
#include "parser.h"
#include "../lib/hr_code.h"

#include <Eigen/Core>
#include <Eigen/Geometry>
using namespace Eigen;

using namespace std;

class UnitConversionException : public exception {};

//namespace Heda {


void dismiss(Heda& heda) {
  heda.waiting_message = "";
  heda.fatal_message = "";
  heda.action_required = "";
  heda.user_response = "";
}

void waitActionDone(Heda& heda) {
  // Loops until the message MESSAGE_DONE has been received.
  while (true) {

    heda.ensureActive();

    if (heda.user_response == MESSAGE_DONE) break;

    this_thread::sleep_for(chrono::milliseconds(10));
  }
  
  dismiss(heda);  
}

void actionIdentify(Heda& heda, DetectedHRCode& code) {

  auto h5 = Header5("ACTION IDENTIFY"); 

  heda.waiting_message = "Un nouveau pot a été détecté. Pouvez-vous svp déterminer ses caractéristiques?";
  heda.action_required = "identify " + to_string(code.id);
  waitActionDone(heda);
}

void writeSlave(Heda& heda, std::string cmd) {
  heda.m_writer << cmd;
  
  // Loops until the message MESSAGE_DONE has been received.
  while (true) {

    heda.ensureActive();

    if (heda.m_reader.inputAvailable()) {

      string str = getInputLine(heda.m_reader);
      if (str == MESSAGE_DONE) break;
    }
      
    this_thread::sleep_for(chrono::milliseconds(10));
  }
}

void actionRaw(Heda& heda) {

  auto h5 = Header5("ACTION RAW"); 

  heda.waiting_message = "Vous pouvez maintenat écrire des commandes à l'esclave.";
  heda.action_required = "raw";
  while (true) {
    heda.ensureActive();

    if (!heda.user_response.empty()) {
      writeSlave(heda, heda.user_response);
      heda.user_response.clear();
    }

    this_thread::sleep_for(chrono::milliseconds(50));
  }
}

void openGrip(Heda& heda, double opening) {

  auto h5 = Header5("OPEN GRIP(" + to_string(opening) + ")"); 

  double mvt = min(opening - heda.config.space_between_jaws, heda.config.max_r);
  ensure(mvt > 0, "Il est impossible de fermer la prise si petit. Ouverture = " + to_string(opening) + ". Minimum = " + to_string(heda.config.space_between_jaws));

  writeSlave(heda, "mr"+to_string(mvt));

  heda.is_gripping = false;
}

void openGrip(Heda& heda, Jar jar) {

  // OPTIMIZE: Keep the jar format in memory for the jar instead of doing this every time.
  JarFormat format = heda.db.find<JarFormat>(jar.jar_format_id);
  ensure(format.exists(), "Putdown needs a valid jar format. The jar did not have one.");

  openGrip(heda, max(format.diameter, format.lid_diameter) + heda.config.grip_gap*2.0);
}

//void openGrip(Heda& heda) {
//
//  auto h5 = Header5("OPEN GRIP"); 
//
//  writeSlave(heda, "r");
//
//  heda.is_gripping = false;
//}

void reference(Heda& heda, Axis& axis) {

  std::string cmd = "h" + string(1, axis.id);
  auto h5 = Header5("HOME("+cmd+")"); 

  writeSlave(heda, cmd);

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

void grab(Heda& heda, double force) {

  std::string cmd = "g" + to_string(force);
  auto h5 = Header5("GRAB("+cmd+")"); 

  writeSlave(heda, cmd);

  // TODO: set is grabbing
}

void move(Heda& heda, Axis& axis, double destination) {

  std::string cmd = "d" + string(1, axis.id) + to_string(destination);
  auto h5 = Header5("MOVE("+cmd+")"); 

  writeSlave(heda, cmd);

  if (axis.id == AXIS_H) {
    heda.m_position.h = destination;
  } else if (axis.id == AXIS_V) {
    heda.m_position.v = destination;
  } else if (axis.id == AXIS_T) {
    heda.m_position.t = destination;
  //} else if (axis.id == AXIS_R) {
  }
}

void validateGoto(Heda& heda, PolarCoord c) {
  ensure(c.h >= 0.0, "The goto destination h must be higher or equal to zero.");
  ensure(c.v >= 0.0, "The goto destination v must be higher or equal to zero.");
  ensure(c.t >= 0.0, "The goto destination t must be higher or equal to zero.");
}

void gotoPolar(Heda& heda, PolarCoord destination) {

  std::string cmd = to_string(destination.h) + " " + to_string(destination.v) + " " + to_string(destination.t);
  auto h4 = Header4("GOTO("+cmd+")");

  validateGoto(heda, destination);

  PolarCoord position = heda.getPosition();

  Shelf currentShelf; heda.shelfByHeight(currentShelf, heda.unitY(position.v));
  Shelf destinationShelf; heda.shelfByHeight(destinationShelf, heda.unitY(destination.v));
    
  double positionT = position.t;

  // must change level
  if (currentShelf.id != destinationShelf.id) {

    move(heda, heda.axisV, heda.unitV(currentShelf.moving_height));

    positionT = (position.h < (heda.config.max_h / 2.0)) ? CHANGE_LEVEL_ANGLE_HIGH : CHANGE_LEVEL_ANGLE_LOW;
    move(heda, heda.axisT, positionT);
  }

  move(heda, heda.axisV, destination.v);

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
      move(heda, heda.axisT, 90.0);
    }

    move(heda, heda.axisH, destination.h);
    move(heda, heda.axisT, destination.t);
  } else {
    move(heda, heda.axisT, destination.t);
    move(heda, heda.axisH, destination.h);
  }
}
        
void gohome(Heda& heda) {
  gotoPolar(heda, PolarCoord(heda.unitH(heda.config.home_position_x, 0, 0), heda.unitV(heda.config.home_position_y), heda.config.home_position_t));
}

Location getNewLocation(Heda& heda, Jar& jar, Shelf& shelf) {

  auto h3 = Header3("GET NEW LOCATION");

  //heda.db.deleteFrom<Location>("WHERE occupied = 0 AND shelf_id = " + to_string(heda.working_shelf.id));  

  JarFormat format = heda.db.find<JarFormat>(jar.jar_format_id);
  ensure(format.exists(), "Get new location needs a valid jar format. The jar did not have one.");

  double widthNeeded = max(format.diameter, format.lid_diameter) + 32; // mm, FIXME: Hardcoded, 32mm

  double minZAccessible = std::max(heda.config.user_coord_offset_z - heda.config.gripper_radius + widthNeeded / 2.0 + 2.0, 0.0);
  double minXAccessible = 100.0; // FIXME hardcoded. Too close to 0 and the arm will collide with the wall.

  vector<Location> locations = heda.db.all<Location>("WHERE shelf_id = " + to_string(shelf.id));

  for (double z = minZAccessible; z < shelf.depth - widthNeeded; z += 10) {
    for (double x = minXAccessible; x < shelf.width - widthNeeded - minXAccessible; x += 10) {

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
        loc.jar_id = jar.id;
        loc.occupied = false;
        heda.db.insert(loc);
        return loc;
      }
    }
  }

  return Location();
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


void detectCodes(Heda& heda, vector<DetectedHRCode>& detected) {
    
  cv::Mat frame;
  heda.captureFrame(frame);

  cout << "Running detect code." << endl;
  vector<HRCode> positions;
  findHRCodes(frame, positions);

  if (!positions.empty()) {
    for (auto it = positions.begin(); it != positions.end(); ++it) {
      cout << "Detected one HRCode!!!" << endl;
      DetectedHRCode d;
      d.h = heda.getPosition().h;
      d.v = heda.getPosition().v;
      d.t = heda.getPosition().t;
      d.centerX = it->x;
      d.centerY = it->y;
      d.scale = it->scale;
      d.img = it->imgFilename;
      detected.push_back(d);
    }
    return;
  }
  cout << "No codes were detected..." << endl;
}

void debugDetect(Heda& heda) {
 
  cv::Mat src = cv::imread("tmp/hrcode/finder/lastCapture.jpg", cv::IMREAD_COLOR);
  vector<HRCode> positions;
  findHRCodes(src, positions);
}

// Maybe use must as a convention for functions that can throw exception?
DetectedHRCode mustDetectOneCode(Heda& heda) {

  vector<DetectedHRCode> detected;
  detectCodes(heda, detected);
  ensure(detected.size() == 1, "There should have been one and only one code detected.");
  return detected.back();
}

void detect(Heda& heda) {
  
  auto h5 = Header5("DETECT");

  vector<DetectedHRCode> detected;
  detectCodes(heda, detected);
  for (DetectedHRCode& it : detected) {
    heda.db.insert(it);
  }
}

void pinpoint(Heda& heda) {
  
  auto h2 = Header2("PINPOINT");

  for (DetectedHRCode& code : heda.db.all<DetectedHRCode>()) {
    pinpointCode(heda, code);
    heda.db.update(code);
  }
}

void parse(Heda& heda) {
  
  auto h2 = Header2("PARSE");

  for (DetectedHRCode& code : heda.db.all<DetectedHRCode>()) {
    parseCode(heda, code);
    heda.db.update(code);
  }
}

class Cluster {
  public:
    int idxKept;
    vector<int> indicesRemoved;
};

double distanceSquare(const UserCoord& c1, const UserCoord& c2) {
  return (c1.x-c2.x)*(c1.x-c2.x) + (c1.y-c2.y)*(c1.y-c2.y) + (c1.z-c2.z)*(c1.z-c2.z); 
}

double detectedDistanceSquared(const DetectedHRCode& c1, const DetectedHRCode& c2) {
  Vector2f lid1; lid1 << c1.lid_x, c1.lid_z;
  Vector2f lid2; lid2 << c2.lid_x, c2.lid_z;
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

void sweep(Heda& heda) {

  auto h2 = Header2("SWEEP");

  heda.db.clear<DetectedHRCode>();
  
  PolarCoord max(heda.config.max_h, heda.config.max_v, 90.0);

  int nStepX = 10;
  int nStepZ = 4;

  double xMin = heda.unitX(0.0, 0.0, 0.0);
  double xMax = heda.unitX(heda.config.max_h, 0.0, 0.0);
  double xDiff = xMax - xMin;

  double zMin = heda.unitZ(90.0, heda.config.gripper_radius);
  double zMax = heda.working_shelf.depth;
  double zDiff = zMax - zMin;
 
  bool xUp = true;
  for (int j = 0; j <= nStepZ; j++) {
    for (int i = 0; i <= nStepX; i++) {

      double x = (i*1.0*xDiff/nStepX)+xMin;
      double z = (j*1.0*zDiff/nStepZ)+zMin;

      if (!xUp) {x = xMax + xMin - x;}

      UserCoord c(x, heda.working_shelf.height+heda.config.detect_height, z);
      gotoPolar(heda, heda.toPolarCoord(c,heda.config.gripper_radius));
      detect(heda);
    }
    xUp = !xUp;
  }

  pinpoint(heda);
  parse(heda);
  gotoPolar(heda, PolarCoord(heda.unitH(heda.config.home_position_x, 0, 0), heda.unitV(heda.config.home_position_y), heda.config.home_position_t));
  removeNearDuplicates(heda);
}

void closeup(Heda& heda, DetectedHRCode& detected) {
  
  auto h2 = Header2("CLOSEUP");

  double robotZ = heda.config.user_coord_offset_z - detected.lid_z;

  if (robotZ > heda.config.camera_radius) {
    robotZ = heda.config.camera_radius;
  }

  double userZ = heda.config.user_coord_offset_z - robotZ;
 
  gotoPolar(heda, heda.toPolarCoord(UserCoord(detected.lid_x, detected.lid_y + heda.config.closeup_distance, userZ), heda.config.camera_radius));

  string errorMsg;
  int maxAttempts = 10;
  for (int i = 0; i < maxAttempts; i++) {
    vector<DetectedHRCode> allDetected;
    detectCodes(heda, allDetected); // TODO: use mustDetectOneCode, attempts=10

    if (allDetected.size() < 1) {errorMsg = "There must be a detected code in a closup."; continue;}
    if (allDetected.size() > 1) {errorMsg = "There must be only one detected code in a closup."; continue;}
    
    DetectedHRCode recent = allDetected[0];

    detected.h = recent.h;
    detected.v = recent.v;
    detected.t = recent.t;
    detected.centerX = recent.centerX;
    detected.centerY = recent.centerY;
    detected.scale = recent.scale;
    detected.img = recent.img;

    pinpointCode(heda, detected);
    parseCode(heda, detected);

    if (detected.jar_id.size() != 3) {errorMsg = "Jar id must have 3 digits, but was '" + detected.jar_id + "'"; continue;}
    
    heda.db.update(detected);

    int id = atoi(detected.jar_id.c_str());
    if (!heda.db.findBy<Jar>("jar_id", id).exists()) {
      actionIdentify(heda, detected);
    }
  
    return;
  }
  ensure(false, errorMsg);
}


void hover(Heda& heda, double x, double z, double reference) {
  
  auto h4 = Header4("HOVER");

  Shelf shelf; heda.shelfByHeight(shelf, heda.unitY(heda.m_position.v));

  UserCoord c(x, shelf.moving_height, z);
  gotoPolar(heda, heda.toPolarCoord(c, reference));
}

void lowerForGrip(Heda& heda, Jar& jar) {

  auto h4 = Header4("LOWER FOR GRIP");

  Shelf shelf; heda.shelfByHeight(shelf, heda.unitY(heda.m_position.v));

  JarFormat format = heda.db.find<JarFormat>(jar.jar_format_id);
  ensure(format.exists(), "Lower for grip needs a valid jar format. The jar did not have one.");
 
  double v = heda.unitV(shelf.height + format.height + heda.config.grip_offset);
  move(heda, heda.axisV, v);
}

void grip(Heda& heda, Jar& jar) {

  auto h5 = Header5("GRIP");

  JarFormat format = heda.db.find<JarFormat>(jar.jar_format_id);
  ensure(format.exists(), "Grip needs a valid jar format. The jar did not have one.");

  grab(heda, format.grip_force);

  heda.is_gripping = true;
}

void putdown(Heda& heda, Jar& jar) {

  auto h4 = Header4("PUTDOWN");

  ensure(heda.is_gripping, "Heda is not gripping so it can't putdown.");
 
  lowerForGrip(heda, jar);
  openGrip(heda, jar);
}

void storeDetected(Heda& heda, DetectedHRCode& detected) {
  
  auto h1 = Header1("STORE DETECTED");

  // Do a closeup first
  closeup(heda, detected);

  // TODO: No need to this this anymore...
  // We must refresh the detected code because it was updated by the closeup and maybe by the action identify too.
  DetectedHRCode updated = heda.db.find<DetectedHRCode>(detected.id);
  ensure(updated.exists(), "Error the previously existing detected hr code was not found anymore. Aborting stored...");
  int jarId = atoi(updated.jar_id.c_str());

  Jar jar = heda.db.findBy<Jar>("jar_id", jarId); 
  ensure(jar.exists(), "A jar should already exists or should have been created by the user in the cloesup. Aborting stored...");

  Location loc = heda.db.findBy<Location>("jar_id", jar.jar_id);
  Shelf shelf;
  if (loc.exists()) {
    shelf = heda.db.find<Shelf>(loc.shelf_id);
  } else {
    order(heda.storage_shelves, byHeight, false);
    for (Shelf& s : heda.storage_shelves) {
      shelf = s;
      loc = getNewLocation(heda, jar, shelf); 
      if (loc.exists()) break;
    }
    ensure(loc.exists(), "Location could not be created. No space on shelves left? Can't save to database?");
  }

  UserCoord c(detected.lid_x, heda.getToolPosition().y, detected.lid_z);
  gotoPolar(heda, heda.toPolarCoord(c, heda.config.gripper_radius));

  openGrip(heda, jar);
  lowerForGrip(heda, jar); 
  grip(heda, jar);

  gotoPolar(heda, heda.toPolarCoord(UserCoord(loc.x,shelf.moving_height,loc.z), heda.config.gripper_radius));
  putdown(heda, jar);

  loc.occupied = true;
  heda.db.update(loc);
  heda.db.remove(detected);
}

void bring(Heda& heda, Ingredient& ingredient) {

  auto h2 = Header2("BRING");

  Jar jar = heda.db.findBy<Jar>("ingredient_id", ingredient.id);
  ensure(ingredient.exists(), "Could not find a jar that contains the ingredient " + ingredient.name);

  // TODO: Make sure the the quantity of ingredients left in the jar is ok.
 
  Location loc = heda.db.findBy<Location>("jar_id", jar.id);
  ensure(loc.exists(), "Could not find the location of the jar id = " + to_string(jar.id));

  Shelf shelf = heda.db.find<Shelf>(loc.shelf_id);
  ensure(shelf.exists(), "Could not find the shelf of the location id = " + to_string(loc.id));
    
  Location dest = getNewLocation(heda, jar, heda.working_shelf); 
  ensure(dest.exists(), "Could not find a destination location to drop the jar brought.");
  
  Shelf destShelf = heda.db.find<Shelf>(dest.shelf_id);
  ensure(destShelf.exists(), "Could not find the shelf of the location id = " + to_string(dest.id));

  gotoPolar(heda, heda.toPolarCoord(UserCoord(loc.x,shelf.moving_height,loc.z), heda.config.gripper_radius));
  openGrip(heda, heda.config.max_r+heda.config.space_between_jaws);
  lowerForGrip(heda,jar);
  grip(heda,jar);

  loc.occupied = false;
  heda.db.update(loc);

  gotoPolar(heda, heda.toPolarCoord(UserCoord(dest.x,destShelf.moving_height,dest.z), heda.config.gripper_radius));
  putdown(heda,jar);

  dest.occupied = true;
  heda.db.update(dest);
}

void fetch(Heda& heda, Recipe& recipe) {

  for (IngredientQuantity ingQty : heda.db.all<IngredientQuantity>("WHERE recipe_id = " + to_string(recipe.id))) {

    Ingredient ingredient = heda.db.find<Ingredient>(ingQty.ingredient_id);
    ensure(ingredient.exists(), "Could not find the ingredient of the ingredient quantity id = " + ingQty.id);

    bring(heda, ingredient);
  }
}

void calibrate(Heda& heda) {

  PolarCoord c = heda.getPosition();
  c.v = heda.unitV(heda.config.camera_calibration_height + heda.working_shelf.height);
  gotoPolar(heda, c);
  DetectedHRCode code = mustDetectOneCode(heda);
  heda.config.camera_focal_point = heda.config.camera_calibration_height * code.scale;
  heda.db.update(heda.config);
}

void moveSpoon(Heda& heda, double angle) {
  
  std::string cmd = "mp" + to_string(angle);
  auto h5 = Header5("MOVE("+cmd+")"); 
  writeSlave(heda, cmd);
}

void shake(Heda& heda) {
  auto h4 = Header4("SHAKE");
  writeSlave(heda, "kh");
}

void scoop(Heda& heda, Jar jar, Spoon spoon) {
  auto h3 = Header3("SCOOP");

  // Move on top of the working jar
  UserCoord c(heda.config.working_x,heda.working_shelf.moving_height,heda.config.working_z);
  gotoPolar(heda, heda.toPolarCoord(c, spoon.radius));

  // Angle the spoon a little backward
  moveSpoon(heda, -10);

  JarFormat format = heda.db.find<JarFormat>(jar.jar_format_id); // OPTIMIZE: Do a mustFind instead
  ensure(format.exists(), "Scoop needs a valid jar format. The jar did not have one.");
 
  // Get down some distance below the jar
  double y = heda.working_shelf.height + format.height + spoon.offset_y - 50.0;
  move(heda, heda.axisV, heda.unitV(y));
  
  // Angle the spoon a little forward
  moveSpoon(heda, 10);
 
  // Go up a little
  y = heda.working_shelf.height + format.height + spoon.offset_y;
  move(heda, heda.axisV, heda.unitV(y));

  // Shake to make sure it will not fall when trying to reach the bowl
  shake(heda);
 
  // Get on top of the bowl
  c = UserCoord(heda.config.bowl_x,heda.working_shelf.moving_height,heda.config.bowl_z);
  gotoPolar(heda, heda.toPolarCoord(c, spoon.radius));
  
  // Drop everything from the spoon
  moveSpoon(heda, -90);
  shake(heda);
}

double getWeight(Heda& heda) {
  auto h = Header5("WEIGHT");
  heda.fixed_writer << "w";
  std::string w = readAllUntilDone(heda.fixed_reader);
  std::cout << w << "\n";
  char* pEnd;
  double val = strtod(w.c_str(), &pEnd);
  ensure(strlen(pEnd) == 0, "Was unable the parse the weight with sentence=" + w);
  return val;
}

double weightJar(Heda& heda, Jar& jar) {
  
  // Get on top of the scale
  UserCoord c(heda.config.balance_x, heda.working_shelf.height + heda.config.balance_offset + jar.getJarFormat().height, heda.config.balance_z);
  gotoPolar(heda, heda.toPolarCoord(c, heda.config.gripper_radius));
  openGrip(heda, jar);
  return getWeight(heda);
}

class HedaController {
  public:


    HedaController(Heda& heda) : heda(heda) {

      // ------- FIXME -------- All those are commands to the HedaController, not to Heda itself (they would not belong in a recipee!)-------------
   
      m_commands["move"] = [&](ParseResult tokens) {
        char axisName = tokens.popAxis();
        double dest = tokens.popScalaire();
       
        Axis* axis = heda.axisByName(axisName);
        ensure(axis != 0, "ref command expects a valid axis name");
        move(heda, *axis, dest);
      };
      
      m_commands["home"] = [&](ParseResult tokens) {
        // TODO: home goes home, and does a referencing if needed only
        auto h1 = Header1("REFERENCING");
        reference(heda, heda.axisR);
        reference(heda, heda.axisT);
        reference(heda, heda.axisH);
        move(heda, heda.axisT, CHANGE_LEVEL_ANGLE_HIGH);
        reference(heda, heda.axisV);
        gohome(heda);
        //openGrip(heda);
      };
      
      //m_commands["process"] = [&](ParseResult tokens) { // Calculate for a recipee by id
      //  unsigned long id = tokens.popPositiveInteger();
      //  Recipe recipe = heda.db.findBy<Recipe>("name", name, "COLLATE NOCASE");
      //}
      m_commands["parserecipe"] = [&](ParseResult tokens) { // Calculate for a recipee by name
        string name = tokens.popNoun();
        Recipe recipe = heda.db.findBy<Recipe>("name", name, "COLLATE NOCASE");
        ensure(recipe.exists(), "parserecipe command must have a valid recipe name");
        parseRecipe(heda.db, recipe);
      };
      
      m_commands["detect"] = [&](ParseResult tokens) {
        DetectedHRCode code = mustDetectOneCode(heda);
        parseCode(heda,code);
        heda.waiting_message = "Pot détecté #" + code.jar_id;
        heda.action_required = "none";
        waitActionDone(heda);

      };
      m_commands["open"] = [&](ParseResult tokens) {
        double opening = tokens.popScalaire();
        openGrip(heda, opening);
      };
      m_commands["raw"] = [&](ParseResult tokens) {actionRaw(heda);};
      m_commands["sweep"] = [&](ParseResult tokens) {sweep(heda);gohome(heda);};
      m_commands["gohome"] = [&](ParseResult tokens) {gohome(heda);};
      m_commands["stop"] = [&](ParseResult tokens) {heda.stop();};
      m_commands["dismiss"] = [&](ParseResult tokens) {dismiss(heda);};
      m_commands["pause"] = [&](ParseResult tokens) {heda.is_paused = true;};
      m_commands["done"] = [&](ParseResult tokens) {heda.user_response = "done";};
      m_commands["unpause"] = [&](ParseResult tokens) {heda.is_paused = false;};
      m_commands["loadcfg"] = [&](ParseResult tokens) {heda.loadConfig();};
      m_commands["pinpoint"] = [&](ParseResult tokens) {pinpoint(heda);};
      m_commands["calibrate"] = [&](ParseResult tokens) {calibrate(heda);};
      m_commands["parse"] = [&](ParseResult tokens) {parse(heda);};
      m_commands["debugdetect"] = [&](ParseResult tokens) {debugDetect(heda);};
      m_commands["shake"] = [&](ParseResult tokens) {shake(heda);};

      m_commands["scoop"] = [&](ParseResult tokens) {

        int id = tokens.popPositiveInteger();
        //Ingredient ingredient = heda.db.find<Ingredient>(id);
        //ensure(ingredient.exists(), "Could not find an ingredient with id = " + to_string(id));

        Jar jar = heda.db.findBy<Jar>("ingredient_id", id);
        ensure(jar.exists(), "Could not find a jar for the ingredient id = " + to_string(id));

        Spoon spoon = heda.db.find<Spoon>(1);
        ensure(spoon.exists(), "Could not find a spoon to scoop with");

        scoop(heda, jar, spoon);
        gohome(heda);
      };

      m_commands["response"] = [&](ParseResult tokens) {
        // FIXME!!! Remove ParseResult, so I get the raw command here
        //heda.user_response = true;
      };
      
      m_commands["storeall"] = [&](ParseResult tokens) { // store all detected jar, starting with the tallest

        for (DetectedHRCode& code : heda.db.all<DetectedHRCode>("ORDER BY lid_y DESC")) {
          storeDetected(heda, code);
        }
        gohome(heda);
      };
      
      m_commands["store"] = [&](ParseResult tokens) { // store all detected jar, starting with the tallest

        sweep(heda);
        
        for (DetectedHRCode& code : heda.db.all<DetectedHRCode>("ORDER BY lid_y DESC")) {
          storeDetected(heda, code);
        }
        gohome(heda);
      };
      
      m_commands["bring"] = [&](ParseResult tokens) { // Bring an ingredient

        string ingredientName = tokens.popNoun();

        Ingredient ingredient = heda.db.findBy<Ingredient>("name", ingredientName, "COLLATE NOCASE");
        ensure(ingredient.exists(), "Could not find an ingredient with the name " + ingredientName);

        bring(heda, ingredient);

        gohome(heda);
      }; 

      m_commands["fetch"] = [&](ParseResult tokens) { // Fetch a recipe

        //string recipeName = tokens.popNoun();
        //Recipe recipe = heda.db.findBy<Recipe>("name", recipeName, "COLLATE NOCASE");

        int id = tokens.popPositiveInteger();
        Recipe recipe = heda.db.find<Recipe>(id);
        ensure(recipe.exists(), "Could not find the recipe with id = " + id);

        fetch(heda, recipe);
        
        gohome(heda);
      }; 
      m_commands["nodup"] = [&](ParseResult tokens) {
        removeNearDuplicates(heda);
      };

      m_commands["test"] = [&](ParseResult tokens) {
        auto h1 = Header1("TEST");
        //Ingredient i;
        //i.name = "FooBar";
        //heda.db.insert(i);
        //ensure(i.exists(), "FooBar must exist!");
        std::cout << getWeight(heda) << std::endl;
        //heda.config.grip_offset = 0.0;
        //heda.db.update(heda.config);
      };

      //// at throws a out of range exception 
      //m_commands["calibrate"] = [&](ParseResult tokens) {
      //  string jarFormatName = tokens.popNoun();

      //  JarFormat format = heda.db.findBy<JarFormat>("name", jarFormatName, "COLLATE NOCASE");
      //  ensure(format.exists(), "Calibrate needs a valid jar format.");
      //  heda.calibrate(format);
      //};
      ////m_commands["closeup"] = [&](ParseResult tokens) { // Move closer to the detected codes to get a better picture.
      ////  debug();
      ////  DetectedHRCode code;
      ////  unsigned long id = tokens.popPositiveInteger();
      ////  ensure(heda.codes.get(code, id), "closeup must have a valid code id");
      ////  Jar jar;
      ////  heda.pushCommand(make_shared<CloseupCommand>(code, jar));
      ////};
      //
      //// hover command => Move on top of the thing in x and z, at the moving height of the shelf.
      //m_commands["hover"] = [&](ParseResult tokens) {
      //  double x = tokens.popScalaire();
      //  double z = tokens.popScalaire();
      //  heda.pushCommand(make_shared<HoverCommand>(x,z,heda.config.gripper_radius));
      //};
      //m_commands["ref"] = [&](ParseResult tokens) {
      //  char axisName = tokens.popAxis();
      //  Axis* axis = heda.axisByName(axisName);
      //  ensure(axis != 0, "ref command expects a valid axis name");
      //  heda.pushCommand(make_shared<ReferencingCommand>(*axis));
      //};
      ////m_commands["store"] = [&](ParseResult tokens) {
      ////  string name = "";
      ////  try {name = tokens.popNoun();} catch (const MissingArgumentException& e) {/*It's ok it's optional.*/}
      ////  heda.pushCommand(make_shared<StoreCommand>(name));
      ////};
      ////m_commands["grip"] = [&](ParseResult tokens) {
      ////  unsigned long id = tokens.popPositiveInteger();
      ////  Jar jar;
      ////  if (heda.jars.get(jar, id)) {
      ////    heda.pushCommand(make_shared<GripCommand>(jar));
      ////  } // TODO Handle error
      ////};
      ////m_commands["pickup"] = [&](ParseResult tokens) {
      ////  // TODO: popLocation? worth it? wait to see...
      ////  int id = tokens.popPositiveInteger();
      ////  string locationName = tokens.popNoun();
      ////  Location loc;
      ////  if (heda.locations.byName(loc, locationName) < 0) {
      ////    cout << "A location name was given, but it was not found. Aborting pickup.";
      ////    return;
      ////  }
      ////  Jar jar;
      ////  if (heda.jars.get(jar, id)) {
      ////    heda.pushCommand(make_shared<PickupCommand>(jar, loc));
      ////    return;
      ////  } // TODO Handle error
      ////  cout << "Oups. No jar were found with this id." << endl;
      ////};
      //m_commands["grab"] = [&](ParseResult tokens) {
      //  double strength = tokens.popScalaire();
      //  cout << "Executing grab with strength = " << strength << endl;
      //  heda.pushCommand(make_shared<GrabCommand>(strength));
      //};
      //m_commands["capture"] = [&](ParseResult tokens) {heda.capture();};
      //m_commands["detect"] = [&](ParseResult tokens) {heda.pushCommand(make_shared<DetectCommand>());};
      //m_commands["putdown"] = [&](ParseResult tokens) {heda.pushCommand(make_shared<PutdownCommand>());};
      //m_commands["photo"] = [&](ParseResult tokens) {
      //  Mat mat;
      //  heda.captureFrame(mat);
      //};
      //m_commands["goto"] = [&](ParseResult tokens) {
      //  double h = tokens.popScalaire();
      //  double v = tokens.popScalaire();
      //  double t = tokens.popScalaire();
      //  heda.pushCommand(make_shared<GotoCommand>(PolarCoord(h,v,t)));
      //};
      //m_commands["getto"] = [&](ParseResult tokens) {
      //  double x = tokens.popScalaire();
      //  double y = tokens.popScalaire();
      //  double z = tokens.popScalaire();
      //  heda.pushCommand(make_shared<GotoCommand>(heda.toPolarCoord(UserCoord(x,y,z), heda.config.gripper_radius)));
      //};
      //m_commands["open"] = [&](ParseResult tokens) {
      //  heda.pushCommand(make_shared<OpenGripCommand>());
      //};
    
    }

    std::function<void(ParseResult)> getCommand(string name) {
       
      for (auto item : m_commands) { // I am not using map[key] because I want to compare without case. OPTIMIZE better data structure to do this.
        if (iequals(item.first, name)) {
          return item.second;
        }
      }
      return 0;
    }

    // Commands can be split with a newline (\n)
    // FIXME: Don't execute the commands as soon as I get them. Stack them. Add a ServerCommand or something to the stack.
    // But check for the stop command.
    void execute(string str) {

      if (str.empty()) {return;}

      string::size_type pos = str.find('\n');
      string cmd = (pos == string::npos) ? str : str.substr(0, pos);

      cerr << "Executing cmd = " << cmd << "\n";

      string cmdName;
      Parser parser;
      ParseResult result;
      parser.parse(result, cmd);

      auto func = getCommand(result.getCommand());
      if (func) {
        try {
          func(result);
        } catch (const StoppedException& e) {
          // Nothing to do
        } catch (const EnsureException& e) {
          heda.fatal_message = e.message;
          heda.stop();
        } catch (const MissingArgumentException& e) {cerr << "Caught a missing argument exception" << endl;}
      } else {cerr << "Error unkown command: " << result.getCommand() << "\n"; return;}

      if (pos != string::npos) {execute(str.substr(pos+1));}
    }

    void loopCommandStack(Reader& reader) {
      while (true) {
        if(reader.inputAvailable()) {
          string cmd = getInputLine(reader);
          execute(cmd);
        }
      }
    }

    std::unordered_map<std::string, std::function<void(ParseResult)>> m_commands;

    Heda& heda;
};

#endif
