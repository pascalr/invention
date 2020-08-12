#ifndef _JAR_PACKER_H
#define _JAR_PACKER_H

#include "jar.h"
#include "position.h"
#include "Model.h"
#include "Database.h"

// This class is responsible with determining the optimal jar placement.
// The current jar positions are stored in the database
class JarPacker {
  public:
  
    virtual void calculateStoreMovements() = 0;
};

// TODO: Save the things into possible locations. Only run this once.
// A Location has:
// A position x, y, z
// A list of other locations which must be empty to be able to move to there?
// A path on how to get it in and out.

// Puts the jars 150 mm (6 inches) appart. (The biggest jar possible.)
class NaiveJarPacker : public JarPacker {
  public:

    // Load shelf data
    // Load jar data
    NaiveJarPacker(Database& db) : db(db) {
    }

    void setup() {
      db.load(shelves);

      calculatePossibleLocations();
    }

    void calculatePossibleLocations() {
      //for (auto it = shelves.items.begin(); it != shelves.items.end(); it++) {
      for (const Shelf& shelf : shelves.items) {
        if (shelf.is_working_shelf) continue;
        int nbCols = shelf.width / 148.0;
        int nbRows = shelf.depth / 148.0;
        for (int i = 0; i < nbCols; i++) {
          for (int j = 0; j < nbRows; j++) {
            CartesianCoord c;
            double x = i*148.0 + (148.0 / 2);
            double z = CLAW_RADIUS - (j*148.0 + (148.0 / 2));
            c << x, shelf.height, z;
            //cout << "Calculated possible jar cartesian at " << c << endl;
            PolarCoord r = toolCartesianToPolar(c);
            //cout << "Calculated possible jar location at " << r << endl;
            possible_locations.push_back(r);
          }
        }
      }
    }

    void calculateStoreMovements() {
    }

  protected:

    std::vector<PolarCoord> possible_locations;
    Database& db;
    ShelfTable shelves;

};

#endif
