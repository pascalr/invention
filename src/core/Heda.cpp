#include "Heda.h"
#include "position.h"

void Heda::sweep() {
  db.clear(codes);
  vector<Movement> mvts;
  calculateSweepMovements(*this, mvts);
  move(mvts);
}
