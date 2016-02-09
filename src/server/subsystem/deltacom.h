/**
 * Delta compression subsystem.
 */
#ifndef SOLEMNSKY_DELTACOM_H
#define SOLEMNSKY_DELTACOM_H

#include "sky/sky.h"
#include "sky/delta.h"

namespace sky {
class DeltaCompress : sky::Subsystem {
private:
  // a bunch of tricky stuff will go here in due time
public:
  DeltaCompress(sky::Sky *sky);
};
}

#endif //SOLEMNSKY_DELTACOM_H
