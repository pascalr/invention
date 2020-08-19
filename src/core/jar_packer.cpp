#include "jar_packer.h"

#include "heda.h"
#include "model.h"
#include "position.h"
#include "database.h"

// Naive Jar Packer makes spaces appart for the largest diameter allowed.
void NaiveJarPacker::generateLocations(Heda& heda) {
  //for (auto it = shelves.items.begin(); it != shelves.items.end(); it++) {
  heda.db.clear(heda.locations);
  double dia = 148.0; // mm
  for (const Shelf& shelf : heda.shelves.items) {

    if (shelf.id == heda.config.working_shelf_id) {continue;}

    int nbCols = shelf.width / dia;
    int nbRows = shelf.depth / dia;
    for (int i = 0; i < nbCols; i++) {
      for (int j = 0; j < nbRows; j++) {
        
        double x = i*dia + (dia / 2);
        double z = j*dia + (dia / 2);
        //UserCoord c(x, shelf.height, z);
        //cout << "Calculated possible jar cartesian at " << c << endl;
        //PolarCoord r = toolCartesianToPolar(c);
        //cout << "Calculated possible jar location at " << r << endl;
        //possible_locations.push_back(r);
        Location l(x, z, "", shelf.id, dia);
        heda.db.insert(heda.locations, l);
      }
    }
  }
}
