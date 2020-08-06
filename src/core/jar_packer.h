#ifndef _JAR_PACKER_H
#define _JAR_PACKER_H

#include "jar.h"
#include "position.h"

// This class is responsible with determining the optimal jar placement.
// The current jar positions are stored in the database
class JarPacker {
  public:
  
    virtual void calculateStoreMovements() = 0;
};

// Puts the jars 6 inches appart. (The biggest jar possible.)
class NaiveJarPacker : public JarPacker {
  public:

    // Load shelf data
    // Load jar data
    void setup() {
      
    }

    void calculateStoreMovements() {
    }

  protected:


};

#endif
