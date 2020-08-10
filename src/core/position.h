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

class PolarCoord : public Vector3d {
};

class CartesianCoord : public Vector3d {
};


// Above which shelf is the arm on?
int calculateLevel(PolarCoord position);

class Movement {
  public:
    Movement(char axis, double destination, std::function<void()> callback) : axis(axis), destination(destination), callback(callback) {}
    Movement(char axis, double destination) : axis(axis), destination(destination) {}
    string str() const;
    char axis;
    double destination;
    std::function<void()> callback;
};

PolarCoord toolCartesianToPolar(const CartesianCoord c);

std::ostream& operator<<(std::ostream &os, const PolarCoord& c);

/*void calculateGoto(vector<Movement> &movements, const PolarCoord position, const CartesianCoord destination) {
  PolarCoord p = toolCartesianToPolar(destination);
  calculateGoto(movements, position, p);
}*/

void addMovementIfDifferent(vector<Movement> &movements, Movement mvt, double currentPosition);

// Does all the heavy logic. Breaks a movement into simpler movements and checks for collisions.
void calculateGoto(vector<Movement> &movements, const PolarCoord position, const PolarCoord destination, std::function<void()> callback = 0);

void calculateMoveCommands(vector<Movement> &movements, const PolarCoord position, const CartesianCoord destination);

Vector2d cameraPosition(Vector2d toolPosition, double angle);

Vector2d jarOffset(Vector2d imgCenter, double angle, double scale);

Vector2d convertToAbsolutePosition(Vector2d toolPosition, double angle, Vector2d imgCenter, double scale);
  
#endif
