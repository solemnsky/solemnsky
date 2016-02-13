/**
 * 'Deltas' between game state structures and Pack<> rules for them. Used in the
 * multiplayer protocol and game recording files.
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
  optional<std::string> nameDelta; // exists on name change

  void apply(Plane &plane);
};

/**
 * Our various PackRules, in namespace sky::pk.
 */
namespace pk {
using namespace tg;

extern const Pack<float> floatPack;
extern const Pack<bool> boolPack;

extern const tg::Pack<sf::Vector2f> vectorPack;

extern const Pack<PlaneTuning::Energy> planeTuningEnergyPack;
extern const Pack<PlaneTuning::Stall> planeTuningStallPack;
extern const Pack<PlaneTuning::Flight> planeTuningFlightPack;
extern const Pack<PlaneTuning> planeTuningPack;

extern const Pack<Clamped> clampedPack;
extern const Pack<Switched> switchedPack;
extern const Pack<Angle> anglePack;

extern const Pack<PlaneVital> planeVitalPack;

}

}

#endif //SOLEMNSKY_DELTA_H
