#include "jar_packer.h"

#include "heda.h"
#include "model.h"
#include "position.h"
#include "database.h"

void NaiveJarPacker::generateLocations(Heda& heda) {
  //for (auto it = shelves.items.begin(); it != shelves.items.end(); it++) {
  heda.db.clear(heda.locations);
  for (const Shelf& shelf : heda.shelves.items) {

    int nbCols = shelf.width / 148.0;
    int nbRows = shelf.depth / 148.0;
    for (int i = 0; i < nbCols; i++) {
      for (int j = 0; j < nbRows; j++) {
        
        double x = i*148.0 + (148.0 / 2);
        double z = j*148.0 + (148.0 / 2);
        //UserCoord c(x, shelf.height, z);
        //cout << "Calculated possible jar cartesian at " << c << endl;
        //PolarCoord r = toolCartesianToPolar(c);
        //cout << "Calculated possible jar location at " << r << endl;
        //possible_locations.push_back(r);
        Location l(x, z, "", shelf.id);
        heda.db.insert(heda.locations, l);
      }
    }
  }
}
