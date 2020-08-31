#ifndef POSITION_H
#define POSITION_H

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

Les coordonnées cartésiennes de l'utilisateur:
X: X0 est à partir de la paroie gauche.
Y: Y0 est la tablette la plus basse.
Z: Z0 est à partir du devant.
*/

class PolarCoord {
  public:
    PolarCoord() {}
    PolarCoord(double h, double v, double t) : h(h), v(v), t(t) {}
    PolarCoord(const PolarCoord& c) : h(c.h), v(c.v), t(c.t) {}
    
    inline bool operator==(const PolarCoord& r){
      return std::tie(h, v, t) == std::tie(r.h, r.v, r.t);
    }

    double h;
    double v;
    double t;
};

class UserCoord {
  public:
    UserCoord() {}
    UserCoord(double x, double y, double z) : x(x), y(y), z(z) {}
    UserCoord(const UserCoord& c) : x(c.x), y(c.y), z(c.z) {}

    inline bool operator==(const UserCoord& r){
      return std::tie(x, y, z) == std::tie(r.x, r.y, r.z);
    }
    
    double x;
    double y;
    double z;
};

double distanceSquare(const UserCoord& c1, const UserCoord& c2);

class Movement {
  public:
    Movement(char axis, double destination, std::function<void()> callback) : axis(axis), destination(destination), callback(callback) {}
    Movement(char axis, double destination) : axis(axis), destination(destination) {}
    string str() const;
    char axis;
    double destination;
    std::function<void()> callback;
};

std::string to_string(const PolarCoord& c);
std::string to_string(const UserCoord& c);

std::ostream& operator<<(std::ostream &os, const PolarCoord& c);
std::ostream& operator<<(std::ostream &os, const UserCoord& c);

void addMovementIfDifferent(vector<Movement> &movements, Movement mvt, double currentPosition);

void doNothing();

#endif
