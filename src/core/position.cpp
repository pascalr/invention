#include "position.h"
#include "heda.h"

#include "../utils/constants.h"

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

//using PolarCoord = Vector3d; // (x,y,t)
//using CartesianCoord = Vector3d; // (x,y,z)

/*
Les coordonées polaires et les coordonées cartésiennes sont vraiment différentes.

Les coordonées polaire:
X: X0 est à la droite du point de vue de l'utilisateur. C'est le maximum que le bras peut se déplacer.
Y: Y0 est le plus bas que la machine peut se déplacer
T: Theta0 est le bras orienté vers la gauche du point de vue de l'utilisateur.

Les coordonnées cartésiennes:
X: X0 est à partir de la paroie gauche.
Y: Y0 est la tablette la plus basse.
Z: Z0 est à partir du devant.
*/


string Movement::str() const {
  stringstream ss; ss << "m" << axis << destination;
  return ss.str();
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

std::ostream& operator<<(std::ostream &os, const PolarCoord& c) {
  return os << "(" << c.h << ", " << c.v << ", " << c.t << ")";
}

std::ostream& operator<<(std::ostream &os, const UserCoord& c) {
  return os << "(" << c.x << ", " << c.y << ", " << c.z << ")";
}

void addMovementIfDifferent(vector<Movement> &movements, Movement mvt, double currentPosition) {
  if (abs(mvt.destination - currentPosition) > 0.001) {
    movements.push_back(mvt);
  }
}

void doNothing() {}

// Does all the heavy logic. Breaks a movement into simpler movements and checks for collisions.
//void calculateGoto(vector<Movement> &movements, const PolarCoord position, const PolarCoord destination, std::function<void()> callback = doNothing) {
void Heda::calculateGoto(vector<Movement> &movements, const PolarCoord position, const PolarCoord destination, std::function<void()> callback) {

  unsigned int size = movements.size();
  // TODO: Collision detection

  int currentLevel = shelfByHeight(toUserHeight(position));
  int destinationLevel = shelfByHeight(toUserHeight(destination));
    
  double positionT = position.t;

  // must change level
  if (currentLevel != destinationLevel) {

    positionT = (position.h < X_MIDDLE) ? CHANGE_LEVEL_ANGLE_HIGH : CHANGE_LEVEL_ANGLE_LOW;
    movements.push_back(Movement('t', positionT));
  }
 
  addMovementIfDifferent(movements, Movement('y', destination.v), position.v); 

  // If moving theta would colide, move x first
  double deltaX = cosd(destination.t) * config.gripper_radius;
  double xIfTurnsFirst = position.h + deltaX;

  if (xIfTurnsFirst < X_MIN || xIfTurnsFirst > X_MAX) {
    addMovementIfDifferent(movements, Movement('x', destination.h), position.h); 
    addMovementIfDifferent(movements, Movement('t', destination.t), positionT); 
  } else {
    addMovementIfDifferent(movements, Movement('t', destination.t), positionT); 
    addMovementIfDifferent(movements, Movement('x', destination.h), position.h); 
  }

  if (callback && movements.size() != size) {
    movements[movements.size()-1].callback = callback;
    //const Movement& oldLast = *movements.end();
    //const Movement& oldLast = movements[movements.size()-1];
    //Movement last = Movement(oldLast.axis, oldLast.destination, callback);
    //movements.pop_back();
    //movements.push_back(last);
  }
}

//void calculateMoveCommands(vector<Movement> &movements, const PolarCoord position, const CartesianCoord destination) {

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
  
//}

