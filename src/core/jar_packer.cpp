#include "jar_packer.h"

#include "heda.h"
#include "model.h"
#include "position.h"
#include "database.h"

class MissingShelfException : public exception {};
    
void NaiveJarPacker::moveToLocation(Heda& heda, Location& loc) {

  Shelf shelf;
  if (!heda.shelves.get(shelf, loc.shelf_id)) {throw MissingShelfException();}

  UserCoord c(loc.x, shelf.moving_height, loc.z);
  PolarCoord p = heda.toPolarCoord(c, heda.config.gripper_radius);
  heda.moveTo(p);
}

int NaiveJarPacker::nextLocation(Heda& heda) {
  for (const Location& loc : heda.locations.items) {
    for (const Jar& jar : heda.jars.items) {
      if (jar.location_id == loc.id) {goto cnt;}
    }
    return loc.id;
    cnt:;
  }
  return -1;
}

// TODO: Order the locations by somethings.

// Naive Jar Packer makes spaces appart for the largest diameter allowed.
void NaiveJarPacker::generateLocations(Heda& heda) {
  //for (auto it = shelves.items.begin(); it != shelves.items.end(); it++) {
  heda.db.clear(heda.locations);
  double dia = 148.0; // mm
  heda.shelves.orderBy(shelfHeightCmp, false); // start with highest shelf (fastest)
  for (const Shelf& shelf : heda.shelves.items) {

    if (shelf.id == heda.config.working_shelf_id) {continue;}

    int nbCols = shelf.width / dia;
    int nbRows = shelf.depth / dia;
    for (int j = 0; j < nbRows; j++) {
      for (int i = 0; i < nbCols; i++) {
        
        double x = i*dia + (dia / 2);
        double z = j*dia + (dia / 2);
        Location l(x, z, "", shelf.id, dia);
        heda.db.insert(heda.locations, l);
      }
    }
  }
}
