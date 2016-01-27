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
    ClassRule<PlaneTuning>::rule(
        {ValueRule<float>::rule(), &PlaneTuning::maxHealth}
    );


const PackRule<PlaneVital> planeVitalPack =
    ClassRule<PlaneVital>::rule(
        {}
    )

const PackRule<PlaneDelta> planeDeltaPack =
    ClassRule<PlaneDelta>::rule(
        MemberRule<PlaneDelta, optional<PlaneTuning>>(
            {OptionalRule<PlaneTuning>::rule(planeTuningPack),
             &PlaneDelta::tuningDelta},
            {OptionalRule<PlaneVital>::rule(planeVitalPack),
            &PlaneDelta::vitalDelta}
        )
    );

}

#endif //SOLEMNSKY_DELTA_H
