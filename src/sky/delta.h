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

  optional<PlaneTuning> tuningDelta; // exists on spawn
  optional<PlaneVital> vitalDelta; // exists on life
  optional<std::string> nameDelta; // exists on name change

  void apply(Plane &plane);
};

/**
 * PackRule's.
 */
const PackRule<PlaneTuning> planeTuningPack =
    ClassRule<PlaneTuning>(
        MemberRule<PlaneTuning, float>(
            ValueRule<float>(), &PlaneTuning::maxHealth)
    );

const PackRule<PlaneVital> planeVitalPack =
    ClassRule<PlaneVital>();

const PackRule<PlaneDelta> planeDeltaPack =
    ClassRule<PlaneDelta>(
//        MemberRule<PlaneDelta, optional<PlaneTuning>>(
//            OptionalRule<PlaneTuning> (planeTuningPack),
//            &PlaneDelta::tuningDelta
//        )
    );
}

#endif //SOLEMNSKY_DELTA_H
