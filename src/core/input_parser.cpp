#include "program.h"
#include "serialize.h"

#include <stdlib.h>

int parseNumber(char** input, double& n) {
  char* pEnd;
  n = strtod(*input, &pEnd);
  // If no number was found
  if (pEnd == *input) {return -1;}
  *input = pEnd;
  return 0;
}

Motor* parseInputMotorAxis(Program& p, char** input, Motor* &axis) {
  char name = **input;

  for (int i = 0; p.motors[i] != 0; i++) {
    if (toupper(name) == p.motors[i]->getName()) {
      axis = p.motors[i];
      (*input)++;
      return axis;
    }
  }

  return 0;
}

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

  Motor* motorAxis;
  double number;
  int status;

  // Prepare
  unsigned long time = p.getCurrentTime();
  for (int i = 0; p.motors[i] != 0; i++) {
    p.motors[i]->prepareWorking(time);
  }

  char* input = p.getInputLine();

  // Move
  if (cmd == 'M' || cmd == 'm') {
    if (!parseInputMotorAxis(p, &input, motorAxis)) {return ERROR_EXPECTED_AXIS;}
    if (parseNumber(&input,number) < 0) {return ERROR_EXPECTED_NUMBER;}
    if ((status = motorAxis->setDestination(number)) < 0) {return status;}

  // Home (referencing) (currently only supports referencing all (not HX or HY)
  } else if (cmd == 'H' || cmd == 'h') {
    doReferencingAll(p);

  // Wait or sleep for some time
  } else if (cmd == 'w' || cmd == 'W') {
    if (parseNumber(&input,number) < 0) {return ERROR_EXPECTED_NUMBER;}
    p.sleepMs(number);

  // Release
  } else if (cmd == 'r' || cmd == 'R') {
    p.axisR.release();

  // Grab
  } else if (cmd == 'g' || cmd == 'G') {
    if (parseNumber(&input,number) < 0) {return ERROR_EXPECTED_NUMBER;}
    p.axisR.grab(number);

  // Move forward
  } else if (cmd == '+') {
    if (!parseInputMotorAxis(p, &input, motorAxis)) {return ERROR_EXPECTED_AXIS;}
    motorAxis->rotate(FORWARD);

  // Move backward
  } else if (cmd == '-') {
    if (!parseInputMotorAxis(p, &input, motorAxis)) {return ERROR_EXPECTED_AXIS;}
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
    } else if (cmd == '@') {
      p.getWriter() << "\n" << MESSAGE_POSITION << "\n";
      p.getWriter() << p.baseAxisX.getPosition() << " "
                    << p.axisY.getPosition() << " "
                    << p.axisT.getPosition();
      
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
