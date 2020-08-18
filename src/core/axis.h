#ifndef AXIS_H
#define AXIS_H

#include "writer/writer.h"
#include "../utils/constants.h"
#include "../utils/utils.h"
#include <math.h>

#define AXIS(t) (axisByLetter(axes, t))

// there is a bug so this is too slow #define MAX_STEP_DELAY 50000 // us (50 ms)
#define MAX_STEP_DELAY 5000 // us (5 ms)

#ifndef ARDUINO
#include <iostream>
using namespace std;
#endif

// I would like to transform the mouvement asked into an optimal path with optimal speed for all axis.
// If I the arm is at z0 and I ask mz340, I expect it to be in a straight line.
// The speed of the x axis will be constant (easier for now) v
// The speed of the theta axis would be (for theta=0 to theta=90) cos(theta)
// The axis all synchronized with the time, so it's ok.
// If they lag behing they will try to go a little faster.

// Y0 is on the floor
// X0 is on the left
// Z0 is aligned with the wheels that go up and down
// The arm starts at theta 0 degree. Counterclockwise is forward.

// TODO: A simple axis has a destination and a position.
// Add a class MotorAxis
// A motor axis has pins and all the stuff.
//
// You cannot go forward or backward with x and z axis, because they are not MotorAxis

class Axis {
  public:
    Axis(Writer& writer, char name) : m_writer(writer) {
      m_name = name;
      m_destination = -1;
      m_max_position = 999999;
      m_min_position = 0;
    }

    char getName() {
      return m_name;
    }
    
    double getDestination() {
      return m_destination;
    }

    virtual double getPosition() = 0;

    virtual int setDestination(double dest) {

      if (dest >= m_max_position + 0.1) {return ERROR_DESTINATION_TOO_HIGH;}
      if (dest <= m_min_position - 0.1) {return ERROR_DESTINATION_TOO_LOW;}

      m_destination = dest;
      updateDirection();

      return 0;
    }
    
    virtual void updateDirection() {}

    void setMaxPosition(double maxP) {
      m_max_position = maxP;
    }

    double getMaxPosition() {
      return m_max_position;
    }
    
    double getMinPosition() {
      return m_min_position;
    }

    void setMinPosition(double pos) {
      m_min_position = pos;
    }

  protected:
    Writer& m_writer;
    char m_name;
    double m_destination;
    double m_max_position;
    double m_min_position;
};

class XAxis : public Axis {
  public:
    XAxis(Writer& writer, char name, Axis& baseXAxis, Axis& thetaAxis) : Axis(writer, name), m_base_x_axis(baseXAxis), m_theta_axis(thetaAxis) {
    }

    double getPosition() {
      return m_base_x_axis.getPosition() + RAYON * cosd(m_theta_axis.getPosition());
    }
    
    void prepare(unsigned long time) {
      setDestination(getPosition());
    }

  private:
    Axis& m_base_x_axis;
    Axis& m_theta_axis;
};

class ZAxis : public Axis {
  public:
    ZAxis(Writer& writer, char name, Axis& thetaAxis) : Axis(writer, name), m_theta_axis(thetaAxis) {
    }

    double getPosition() {
      return RAYON * sind(m_theta_axis.getPosition());
    }

    void prepare(unsigned long time) {
      setDestination(getPosition());
    }
    
  private:
    Axis& m_theta_axis;
};

Axis* axisByLetter(Axis** axes, char letter);

#endif
