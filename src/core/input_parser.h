#ifndef INPUT_PARSER_H
#define INPUT_PARSER_H

#include "program.h"
#include "serialize.h"

int parseNumber(char** input, double& n);
//int parseNumber(Program& p, double& n);

Motor* parseInputMotorAxis(Program& p, char** input, Motor* &axis);
//Axis* parseInputAxis(Program& p, Axis* &axis);

int parseActionCommand(char cmd, Program& p);

void myLoop(Program& p);

#endif
