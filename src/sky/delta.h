/**
 * 'Deltas' between game state structures. Used in the multiplayer protocol and
 * game recording files.
 */
#ifndef SOLEMNSKY_DELTA_H
#define SOLEMNSKY_DELTA_H

#include "flight.h"
#include "telegraph/pack.h"

namespace sky {

/**
 * Delta between two Planes
 */
struct PlaneDelta {
  PlaneDelta() = default; // for unpacking

  optional<PlaneTuning> tuningDelta; // exists on spawn
  optional<PlaneVital> vitalDelta; // exists on life

  void apply(Plane &plane);
};

struct PlaneDeltaPack: public tg::ClassPack<PlaneDelta> {
  PlaneDeltaPack();
};

}

#endif //SOLEMNSKY_DELTA_H
