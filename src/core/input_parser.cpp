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

Motor* parseInputMotorAxis(Program& p, Motor* &axis) {
  char name = p.getChar();

  for (int i = 0; p.motors[i] != 0; i++) {
    if (toupper(name) == p.motors[i]->getName()) {
      axis = p.motors[i];
      return axis;
    }
  }

  return 0;
}

Axis* parseInputMovingAxis(Program& p, Axis* &axis) {
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
/*int processMoveCommand(Program& p) {

  // If asking to move axis theta directly, dont overwrite it.
  if (p.axisT.getPosition() != p.axisT.getDestination()) {
    //p.axisX.setDestination(p.baseAxisX.getPosition() - RAYON * cosd(angleDest);
    //p.axisZ.setDestination(...)
    return 0;
  }

  // asin returns the principal value between -pi/2 and pi/2
  double angleDest = (asin(p.axisZ.getDestination() / RAYON) * 180.0 / PI);

  // we must respect these conditions:
  // 1. 

  // first check which way we were going already
  // then check if we can continue to go that way
  bool lookingRight = p.axisT.getPosition() < 90.0;
  if (lookingRight) {
    angleDest = 180 - angleDest;
  }

  double baseDest = p.axisX.getDestination() - RAYON * cosd(angleDest);
  
  if (p.baseAxisX.setDestination(baseDest) < 0) {
    // try flipping
    angleDest = 180 - angleDest;
    baseDest = p.axisX.getDestination() - RAYON * cosd(angleDest);
    int status = p.baseAxisX.setDestination(baseDest) < 0;
    if (status < 0) {return status;}
  }
  int status = p.axisT.setDestination(angleDest);
  if (status < 0) {return status;}

  //unsigned long timeX = p.baseAxisX.timeToReachDestinationUs();
  //unsigned long timeT = p.axisT.timeToReachDestinationUs();

  // Check for possible collision if the base x axis si too slow to move (It is slower than theta)
  //double possibleX = p.baseAxisX.getPosition() + RAYON * cosd(angleDest);
  //if (possibleX < 0 || possibleX > AXIS_X_MAX_POS) {
  //}
  // Calculate time until it's clear

  //unsigned long timeX = p.baseAxisX.timeToReachDestinationUs();
  //unsigned long timeT = p.axisT.timeToReachDestinationUs();
  
  //MotorAxis& axisToSlowDown = (timeX > timeT) ? p.axisT : p.baseAxisX;
  //unsigned long maxTime = (timeX > timeT) ? timeX : timeT;

  //cout << "timeX: " << p.baseAxisX.timeToReachDestinationUs() / 1000000.0 << endl;
  //cout << "timeT: " << p.axisT.timeToReachDestinationUs() / 1000000.0 << endl;

  //slowDownAxis(axisToSlowDown, maxTime);

  //cout << "After slow down:" << endl;
  //cout << "timeX: " << p.baseAxisX.timeToReachDestinationUs() / 1000000.0 << endl;
  //cout << "timeT: " << p.axisT.timeToReachDestinationUs() / 1000000.0 << endl;

  return 0;

  // check for out of bounds min
  if (baseDest < p.baseAxisX.getMinPosition()) {
    if (!lookingRight) {return ERROR_OUT_OF_BOUNDS_MIN_X;}
    // try flipping
    angleDest = 180 - angleDest;
    baseDest = p.axisX.getDestination() + RAYON * cosd(angleDest);
    if (baseDest < p.baseAxisX.getMinPosition()) {return ERROR_OUT_OF_BOUNDS_MIN_X;}

  // check for out of bounds max
  } else if (baseDest > p.baseAxisX.getMaxPosition()) {
    if (lookingRight) {return ERROR_OUT_OF_BOUNDS_MAX_X;}
    // try flipping
    angleDest = 180 - angleDest;
    baseDest = p.axisX.getDestination() + RAYON * cosd(angleDest);
    if (baseDest > p.baseAxisX.getMaxPosition()) {return ERROR_OUT_OF_BOUNDS_MAX_X;}
  }
}*/

// Référencement: 2 situations:
// - Le bras est orienté de manière à ce qu'il n'y ait pas de collision pour monter descendre:
//     => Faire le référencement de l'axe vertical, puis descendre à la table de travail, faire les autres
//        ( Parce que si le bras était en train de monter ou de descendre, il ne peut pas tourner s'il
//        est vis à vis une tablette.) TODO: edge case, later, not a priority
// - Sinon:
//     => Orienter le bras vers l'avant. Référencer l'axe horizontal, puis le vertical.
void doReferencingAll(Program& p) {
  p.getWriter() << "Referencing...\n";
  p.axisR.startReferencing();
  p.axisT.startReferencing(); // TODO: Use the potentiometer to do the referencing. Move to 90 degrees.
  p.axisT.doneWorkingCallback = [](Program& p) {
    p.baseAxisX.startReferencing();
    p.baseAxisX.doneWorkingCallback = [](Program& p) {
      p.axisY.startReferencing();
    };
  };
  //for (int i = 0; p.motors[i] != 0; i++) {
  //  p.motors[i]->startReferencing();
  //}
}

int parseActionCommand(char cmd, Program& p) {

  Axis* axis;
  Motor* motorAxis;
  double number;
  int status;

  // Prepare
  unsigned long time = p.getCurrentTime();
  for (int i = 0; p.motors[i] != 0; i++) {
    p.motors[i]->prepareWorking(time);
  }
  p.axisX.prepare(time);
  p.axisZ.prepare(time);

  // Move
  if (cmd == 'M' || cmd == 'm') {
    // TODO: Parse mx10y20z30, the movement should be done diagonally at once.
    // later call p.stopMoving whenever an error occur, because
    // if multiple destinations are set, they must be unset if one of them
    // is an error.
    // But right now it does not work I don't know why. There is a bug
    // that it waits for a char???
    //if (!parseInputMovingAxis(p, axis)) {p.stopMoving(); return ERROR_EXPECTED_AXIS;}
    //if (parseNumber(p,number) < 0) {p.stopMoving(); return ERROR_EXPECTED_NUMBER;}
    //if ((status = axis->setDestination(number)) < 0) {p.stopMoving(); return status;}
    //if ((status = processMoveCommand(p)) < 0) {p.stopMoving(); return status;}
    if (!parseInputMovingAxis(p, axis)) {return ERROR_EXPECTED_AXIS;}
    if (parseNumber(p,number) < 0) {return ERROR_EXPECTED_NUMBER;}
    if ((status = axis->setDestination(number)) < 0) {return status;}
    //if ((status = processMoveCommand(p)) < 0) {return status;}

  // Home (referencing) (currently only supports referencing all (not HX or HY)
  } else if (cmd == 'H' || cmd == 'h') {
    doReferencingAll(p);

  // Wait or sleep for some time
  } else if (cmd == 'w' || cmd == 'W') {
    if (parseNumber(p,number) < 0) {return ERROR_EXPECTED_NUMBER;}
    p.sleepMs(number);

  // Release
  } else if (cmd == 'r') {
    p.axisR.release();

  // Grab
  } else if (cmd == 'g') {
    if (parseNumber(p,number) < 0) {return ERROR_EXPECTED_NUMBER;}
    p.axisR.grab(number);

  // Move forward
  } else if (cmd == '+') {
    if (!parseInputMotorAxis(p, motorAxis)) {return ERROR_EXPECTED_AXIS;}
    parseNumber(p,number);
    motorAxis->rotate(FORWARD);

  // Move backward
  } else if (cmd == '-') {
    if (!parseInputMotorAxis(p, motorAxis)) {return ERROR_EXPECTED_AXIS;}
    motorAxis->rotate(REVERSE);
  }

  p.getWriter() << "Cmd: " << cmd << "\n";

  return 0;
}

void myLoop(Program& p) {
  if (p.getReader().inputAvailable()) {
    int incomingByte = p.getReader().getByte();
    if (incomingByte < 0) {
      return;
    }
    char cmd = (char) incomingByte;

    // stop
    if (cmd == 's' || cmd == 'S') {
      p.stopMoving();

    // info
    } else if (cmd == '?') { // info
      p.getWriter() << "\nTime: " << p.getCurrentTime();
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
        p.getWriter() << "Exception: Code: " << code << ".\n";
        /*while (true) { Doesnt work yet because parseActionCommand might have read the \n already
          char throwAway = p.getChar();
          if (throwAway == '\n') {break;}
          p.getWriter() << throwAway;
        }
        p.getWriter() << ".\n";*/
      }
      return;
    }
  }
  if (!p.isWorking) {
    p.sleepMs(10);
    return;
  }

  bool stillWorking = false;
  for (int i = 0; p.motors[i] != 0; i++) {
    stillWorking = p.motors[i]->work(p, p.getCurrentTime()) || stillWorking;
  }

  if (!stillWorking) {
    p.isWorking = false;
    p.getWriter() << MESSAGE_DONE << '\n';
  }
}
