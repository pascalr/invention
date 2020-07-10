#include "axis.h"

#include <cctype> 

Axis* axisByLetter(Axis** axes, char letter) {
  for (int i = 0; axes[i] != NULL; i++) {
    if (toupper(letter) == axes[i]->name) {
      return axes[i];
    }
  }

  return NULL;
}
