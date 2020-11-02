#ifndef UTILS_H
#define UTILS_H

#include "constants.h"
#include <string.h>
#include <math.h>

// Safe way to compute the difference between the currentTime and the oldTime.
// It detects if the time has wrapped around (overflow unsigned long).
unsigned long timeDifference(unsigned long oldTime, unsigned long currentTime);

void rtrim(char* str);

#endif
