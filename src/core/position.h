#ifndef POSITION_H
#define POSITION_H

#include "../config/constants.h"

// COORDINATES:
// Where does the 0 starts?
// x 0 is at middle of carriage
// y 0 is at height of lowest shelf
// z 0 is at middle of carriage

// TODO: Only keep Eigen/Core in header file
#include <Eigen/Core>
#include <Eigen/Geometry>

#include <vector>

using namespace Eigen;
using namespace std;

using PolarCoord = Vector3d; // (x,y,t)
using CartesianCoord = Vector3d; // (x,y,z)


// Above which shelf is the arm on?
int calculateLevel(PolarCoord position) {

  double heights[SHELVES_TOTAL] = SHELVES_HEIGHT;

  for (int i = 0; i < SHELVES_TOTAL; i++) {

    if (position(1) < heights[i]) {
      return i - 1;
    }
  }

  return heights[SHELVES_TOTAL - 1];
}

class Movement {
  public:
    Movement(char axis, double destination) : axis(axis), destination(destination) {}
    string str() {
      stringstream ss; ss << "m" << axis << destination;
      return ss.str();
    }
    char axis;
    double destination;
};

PolarCoord toolCartesianToPolar(const CartesianCoord c) {

  double t = (asin(c(2) / CLAW_RADIUS) * 180.0 / PI);
  if (c(0) > X_MIDDLE) {
    t = 180 - t;
  }
  double deltaX = cosd(t) * CLAW_RADIUS;
  PolarCoord r;
  r << c(0)-deltaX, c(1), t;
  return r;
} 

std::ostream& operator<<(std::ostream &os, const PolarCoord& c) {
  return os << "(" << c(0) << ", " << c(1) << ", " << c(2) << ")";
}

// Does all the heavy logic. Breaks a movement into simpler movements and checks for collisions.
void calculateGoto(vector<Movement> &movements, const PolarCoord position, const PolarCoord destination) {

  // TODO: Collision detection

  int currentLevel = calculateLevel(position);
  int destinationLevel = calculateLevel(destination);

  // must change level
  if (currentLevel != destinationLevel) {

    double angleDest = ((position(0)) > X_MIDDLE) ? CHANGE_LEVEL_ANGLE_HIGH : CHANGE_LEVEL_ANGLE_LOW;
    movements.push_back(Movement('t', angleDest));
  }
  
  movements.push_back(Movement('y', destination(1)));

  // If moving theta would colide, move x first
  double deltaX = cosd(destination(2)) * CLAW_RADIUS;
  double xIfTurnsFirst = position(0) + deltaX;

  if (xIfTurnsFirst < X_MIN || xIfTurnsFirst > X_MAX) {
    movements.push_back(Movement('x', destination(0)));
    movements.push_back(Movement('t', destination(2)));
  } else {
    movements.push_back(Movement('t', destination(2)));
    movements.push_back(Movement('x', destination(0)));
  }
}

void calculateMoveCommands(vector<Movement> &movements, const PolarCoord position, const CartesianCoord destination) {

  // TODO: Transform the CartesianCoord destination into a PolarCoord
  // then call calculateGoto()

  /*int currentLevel = calculateLevel(position);
  int destinationLevel = calculateLevel(destination);

  // must change level
  if (currentLevel != destinationLevel) {

    double angleDest = ((position(0)) > X_MIDDLE) ? CHANGE_LEVEL_ANGLE_HIGH : CHANGE_LEVEL_ANGLE_LOW;
    movements.push_back(Movement('T', CHANGE_LEVEL_ANGLE_HIGH));
  }

  movements.push_back(Movement('Y', destination(1)));

  double angleDest = (asin(destination(2) / CLAW_RADIUS) * 180.0 / PI);
  if (destination(0) > X_MIDDLE) {
    angleDest = 180 - angleDest;
  }
  
  double xDest = destination(0) - CLAW_RADIUS * cosd(angleDest);

  //bool xMovingForward = xDest -*/
  
}


Vector2d cameraPosition(Vector2d toolPosition, double angle) {

  double offset = CAMERA_TOOL_DISTANCE;
  Vector2d cameraOffset(offset*cosd(angle),offset*sind(angle));
  return toolPosition - cameraOffset;
}

Vector2d jarOffset(Vector2d imgCenter, double angle, double scale) {

  Vector2d imgCenterOffset = imgCenter - Vector2d(CAMERA_WIDTH/2, CAMERA_HEIGHT/2) ;

  // Gauche droite sur l'image (donc jarCenter.x) c'est les z quand l'angle est à 0.
  // jarCenter.x positif c'est z négatif.
  // jarCenter.y positif c'est x négatif.
  Vector2d jarCenter;
  jarCenter << -imgCenterOffset(1), -imgCenterOffset(0);

  jarCenter /= scale; // Translate pixels dimensions into mm.

  // I must rotate the offset value because the camera is rotated.
  Transform<double, 2, TransformTraits::Affine> t(Rotation2D<double>(angle / 180.0 * PI));
  jarCenter = t * jarCenter;

  return jarCenter;
}

Vector2d convertToAbsolutePosition(Vector2d toolPosition, double angle, Vector2d imgCenter, double scale) {
  
  Vector2d cameraPos = cameraPosition(toolPosition, angle);

  return cameraPos + jarOffset(imgCenter, angle, scale);
}

#endif
