#include "program.h"
#include "serialize.h"

#include <stdlib.h>

int parseNumber(Program& p, double& n) {

  char number[MAX_NUMBER_CHAR + 1];
  bool periodFound = false;

  int i = 0;
  for(; i < MAX_NUMBER_CHAR; i++) {
    char c = p.getChar();

    // ignore whitespaces
    if (c == ' ') {
      i--;

    // digits are always ok
    } else if (c >= '0' && c <= '9') {
   
    // sign at beginning is ok 
    } else if (i == 0 && (c == '-' || c == '+')) {

    // period found
    } else if (!periodFound && c == '.') {
      periodFound = true;
    
    // end of number
    } else {
      break;
    }
    number[i] = c;
  }
  if (i == 0) {
    return -1;
  }
  number[i+1] = '\0';
  n = atof(number);
  return 1;
}

Axis* parseInputAxis(Program& p, Axis* &axis) {
  char name = p.getChar();
  axis = axisByLetter(p.movingAxes, name);
  return axis;
}

// Do I need a Zaxis or not? The idea is that I have to check for flips too.
// But the nice thing about Zaxis is simply to behave the same way as the others.

// Calculates whether a flip is required and sets the proper speeds.
// Gives a speed function and an amount of time to run.
// Later, the speed function will accelerate and deccelerate.
// Parse all the movement asked. Because it must be able to do mx10y20z30
// This will check if a flip is required, but this will NOT check if it must
// hide the arm to change level to go higher or lower, this should have
// been done before. It should not be done on the arduino.
/*int parseMoveCommand(Program& p) {

  bool zDestinationSet = false;
  double zDestination;
  while (true) {
    char name = p.getChar();
    if (name == 'z' || name == 'Z') {
    } else {
      Axis* axis = axisByLetter(p.axes, name);
      if (!parseInputAxis(p, axis)) {return ERROR_EXPECTED_AXIS;}
      if (parseNumber(p,number) < 0) {return ERROR_EXPECTED_NUMBER;}
      axis->setDestination(number);
    }
  }
  return axis;

}*/

/*bool baseDestinationOutOfBounds() {
  double tipPositionX = m_horizontal_axis->getDestination();
  
  double basePositionX = tipPositionX - (RAYON * cos(getDestinationAngle() / 180 * PI));

  return (basePositionX < 0 || basePositionX > AXIS_X_MAX_POS);
}

void flip() {
  if(m_destination_angle < 90) {
    m_destination_angle = 180 - m_destination_angle;
  } else {
    m_destination_angle = 90 - (m_destination_angle - 90);
  }
  updateDirection();
}

    bool baseNotGoingOutOfBounds() {
      double p = Axis::getPosition();
      return isForward ? p <= getMaxPosition() : p >= m_min_position;
    }*/
// Checks if a flip is required.
// Tells the Zaxis which way to turn.
// Tells all the axis what speed to go to.
// Sets the base destination of the horizontal axis
// Sets the angle destination of the Zaxis
// Checks for collisions.
// Does the heavy work.
// Maybe the validation is only done in a simulation to avoid
// overloading the arduino. Yeah I think so. To be done later.
int processMoveCommand(Program& p) {

  bool mustFlip = false;

  // asin returns the principal value between -pi/2 and pi/2
  double angleDest = (asin(p.axisZ.getDestination() / RAYON) * 180.0 / PI);

  // we must then check if it should be on the other side
  // first check which way we were going already
  // then check if we can continue to go that way

  bool isRight = p.axisZ.getPositionAngle() > modulo regarder quel quartier...

  if (p.axisX.getDestination() > BASE_X_MIDDLE)

  // If tool tip destination is outside of bounding box, flip

  double baseDest = ???;
  if baseDest out of bounds flip 

  p.baseAxisX.setDestination();

  // If the arm is on the left (x=0)
  if (p.axisX.getDestination() < RAYON) {
    // ERROR OUT OF BOUNDS
    mustFlip = true;
  }
  
  if (baseDestinationOutOfBounds()) {
    flip();
  }
}

int parseActionCommand(char cmd, Program& p) {

  Axis* axis;
  double number;

  // Prepare
  for (int i = 0; p.motorAxes[i] != 0; i++) {
    p.motorAxes[i]->prepare(p.getCurrentTime());
  }

  // Move
  if (cmd == 'M' || cmd == 'm') {
    // TODO: Parse mx10y20z30, the movement should be done diagonally at once.
    if (!parseInputAxis(p, axis)) {return ERROR_EXPECTED_AXIS;}
    if (parseNumber(p,number) < 0) {return ERROR_EXPECTED_NUMBER;}
    axis->setDestination(number);
    processMoveCommand(p);

  // Home (referencing) (currently only supports referencing all (not HX or HY)
  } else if (cmd == 'H' || cmd == 'h') {
    p.getWriter() << "Referencing...\n";
    for (int i = 0; p.motorAxes[i] != 0; i++) {
      p.motorAxes[i]->startReferencing();
    }

  // Wait or sleep for some time
  } else if (cmd == 'w' || cmd == 'W') {
    if (parseNumber(p,number) < 0) {return ERROR_EXPECTED_NUMBER;}
    p.sleepMs(number);

  // Move forward
  } else if (cmd == '+') {
    if (!parseInputAxis(p, axis)) {return ERROR_EXPECTED_AXIS;}
    axis->rotate(FORWARD);

  // Move backward
  } else if (cmd == '-') {
    if (!parseInputAxis(p, axis)) {return ERROR_EXPECTED_AXIS;}
    axis->rotate(REVERSE);
  }

  p.getWriter() << "Cmd: " << cmd << "\n";

  return 0;
}

void myLoop(Program& p) {
  if (p.inputAvailable()) {
    int incomingByte = p.getByte();
    if (incomingByte < 0) {
      return;
    }
    char cmd = (char) incomingByte;

    // stop
    if (cmd == 's' || cmd == 'S') {
      for (int i = 0; p.motorAxes[i] != 0; i++) {
        p.motorAxes[i]->stop();
      }
      p.getWriter() << "Stopped\n";
      p.isWorking = false; // Maybe not necessary because already told the axes to stop. Anyway it does not hurt..

    // info
    } else if (cmd == '?') { // info
      p.getWriter() << "\n" << MESSAGE_JSON << "\n";
      serialize<Writer>(p, p.getWriter());
      p.getWriter() << "\n";

    // position (faster than info)
    } else if (cmd == '@') { // TODO
      
    // ignore
    } else if (cmd == '\r' || cmd == '\n') {
    
    // others are action command, so the program should not be working already..
    } else if (p.isWorking) {
      p.getWriter() << "Error, received an action command when previous was not over. Cmd = " << cmd << '\n';
    } else {
      p.isWorking = true;
      int code = parseActionCommand(cmd, p);
      if (code < 0) {
        p.getWriter() << "Exception: Code: " << code << '\n';
      }
      return;
    }
  }
  if (!p.isWorking) {
    p.sleepMs(10);
    return;
  }

  bool stillWorking = false;
  for (int i = 0; p.motorAxes[i] != 0; i++) {
    stillWorking = stillWorking || p.motorAxes[i]->handleAxis(p.getCurrentTime());
  }

  if (!stillWorking) {
    p.isWorking = false;
    p.getWriter() << MESSAGE_DONE << '\n';
  }
}
