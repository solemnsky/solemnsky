/**
 * Datatypes that represent
 */
#ifndef SOLEMNSKY_DELTA_H
#define SOLEMNSKY_DELTA_H

#include "flight.h"
#include "util/packer.h"

namespace sky {

/**
 * Delta between two Planes
 */
struct PlaneDelta {
  PlaneDelta() = default; // for unpacking

  optional<PlaneTuning> tuneDelta; // exists on spawn
  optional<PlaneVital> vitalDelta; // exists on life

  void apply(Plane &vital) { }
};

struct PlaneVitalPack {
};

struct PlaneDeltaPack {
  static PackRule<PlaneDelta> rules() {
    return ClassRule<PlaneDelta>::rules();
  }
};

}

#endif //SOLEMNSKY_DELTA_H
