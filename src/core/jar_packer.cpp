#include "jar_packer.h"

#include "heda.h"
#include "model.h"
#include "position.h"
#include "database.h"

class MissingShelfException : public exception {};
    
HedaCommandPtr NaiveJarPacker::moveToLocationCmd(Heda& heda, const Location& loc) {

  Shelf shelf;
  if (!heda.shelves.get(shelf, loc.shelf_id)) {throw MissingShelfException();}

  UserCoord c(loc.x, shelf.moving_height, loc.z);
  return make_shared<GotoCommand>(heda.toPolarCoord(c, heda.config.gripper_radius));
}

int NaiveJarPacker::nextLocation(Heda& heda, bool storageWanted) {
  for (const Location& loc : heda.locations.items) {
    if (loc.is_storage != storageWanted) {continue;}
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
  heda.shelves.order(byHeight, false); // start with highest shelf (fastest)

  char shelfLetter = 'A';
  for (const Shelf& shelf : heda.shelves.items) {

    int nbCols = shelf.width / dia;
    int nbRows = shelf.depth / dia;

    int locationNumber = 1;
    for (int j = 0; j < nbRows; j++) {
      for (int i = 0; i < nbCols; i++) {
        
        double x = i*dia + (dia / 2);
        double z = j*dia + (dia / 2);
        Location l(x, z, "", shelf.id, dia);
        l.is_storage = shelf.id != heda.config.working_shelf_id;
        l.name = shelfLetter; l.name += to_string(locationNumber);
        heda.db.insert(heda.locations, l);
        locationNumber++;
      }
    }
    shelfLetter++;
  }
}
