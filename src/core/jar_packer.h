#ifndef _JAR_PACKER_H
#define _JAR_PACKER_H

#include "heda.h"

// This class is responsible with determining the optimal jar placement.
// The current jar positions are stored in the database
class JarPacker {
  public:
  
    virtual void generateLocations(Heda& heda) = 0;
    virtual int nextLocation(Heda& heda, bool storageWanted=true) = 0;
    virtual void moveToLocation(Heda& heda, Location& loc) = 0;
};

// TODO: Save the things into possible locations. Only run this once.
// A Location has:
// A position x, y, z
// A list of other locations which must be empty to be able to move to there?
// A path on how to get it in and out.

// Puts the jars 150 mm (6 inches) appart. (The biggest jar possible.)
class NaiveJarPacker : public JarPacker {
  public:

    void generateLocations(Heda& heda);
    int nextLocation(Heda& heda, bool storageWanted=true);
    void moveToLocation(Heda& heda, Location& loc);
};

#endif
