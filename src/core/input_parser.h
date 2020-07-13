#ifndef INPUT_PARSER_H
#define INPUT_PARSER_H

#include "program.h"
#include "axis.h"
#include "serialize.h"

int parseNumber(Program& p, double& n);

Axis* parseInputAxis(Program& p, Axis* &axis);

int parseActionCommand(char cmd, Program& p);

void myLoop(Program& p);

#endif
