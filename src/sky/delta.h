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
 * Our various PackRules, in namespace sky::pk.
 */
namespace pk {
const PackRule<float> floatPack = ValueRule<float>();

const PackRule<sf::Vector2f> vectorPack =
    ClassRule<sf::Vector2f>(
        MemberRule<sf::Vector2f, float>(floatPack, &sf::Vector2f.x),
        MemberRule<sf::Vector2f, float>(floatPack, &sf::Vector2f.y)
    );

const PackRule<PlaneTuning::Energy> planeTuningEnergyPack =
    ClassRule<PlaneTuning::Energy>(
        MemberRule<PlaneTuning::Energy, float>(
            floatPack, &PlaneTuning::Energy::thrustDrain),
        MemberRule<PlaneTuning::Energy, float>(
            floatPack, &PlaneTuning::Energy::recharge),
        MemberRule<PlaneTuning::Energy, float>(
            floatPack, &PlaneTuning::Energy::laserGun)
    );

const PackRule<PlaneTuning> planeTuningPack =
    ClassRule<PlaneTuning>(
        MemberRule<PlaneTuning, sf::Vector2f>(
            vectorPack, &PlaneTuning::hitbox),
        MemberRule<PlaneTuning, PlaneTuning::Energy>(
            planeTuningEnergyPack, &PlaneTuning::energy)
    );

const PackRule<PlaneVital> planeVitalPack =
    ClassRule<PlaneVital>();

const PackRule<PlaneDelta> planeDeltaPack =
    ClassRule<PlaneDelta>(
        MemberRule<PlaneDelta, optional<PlaneTuning>>(
            OptionalRule<PlaneTuning>(planeTuningPack),
            &PlaneDelta::tuningDelta
        )
    );
}
}

#endif //SOLEMNSKY_DELTA_H
