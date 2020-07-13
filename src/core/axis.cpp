#include "axis.h"

#include <ctype.h> 

Axis* axisByLetter(Axis** axes, char letter) {
  for (int i = 0; axes[i] != 0; i++) {
    if (toupper(letter) == axes[i]->name) {
      return axes[i];
    }
  }

  return 0;
}
