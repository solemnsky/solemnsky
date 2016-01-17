/**
 * Delta compression subsystem.
 */
#ifndef SOLEMNSKY_DELTACOM_H
#define SOLEMNSKY_DELTACOM_H

#include "sky/subsystem.h"
#include "sky/delta.h"

namespace sky {

class DeltaCompress : Subsystem {
private:
  // a bunch of tricky stuff will go here in due time
public:
  DeltaCompress(Sky *sky);
};

}

#endif //SOLEMNSKY_DELTACOM_H
