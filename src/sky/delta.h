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

/**** float ****/
const PackRule<float> floatPack = ValuePack<float>();

/**** sf::Vector2f ****/
const PackRule<sf::Vector2f> vectorPack =
    ClassPack<sf::Vector2f>(
        MemberRule<sf::Vector2f, float>(floatPack, &sf::Vector2f::x),
        MemberRule<sf::Vector2f, float>(floatPack, &sf::Vector2f::y)
    );

/**** PlaneTuning::Energy ****/
#define member(TYPE, RULE, PTR) \
  MemberRule<PlaneTuning::Energy, TYPE>(RULE, &PlaneTuning::Energy::PTR)
const PackRule<PlaneTuning::Energy> planeTuningEnergyPack =
    ClassPack<PlaneTuning::Energy>(
        member(float, floatPack, thrustDrain),
        member(float, floatPack, recharge),
        member(float, floatPack, laserGun)
    );
#undef member

/**** PlaneTuning::Stall ****/
#define member(TYPE, RULE, PTR) \
  MemberRule<PlaneTuning::Stall, TYPE>(RULE, &PlaneTuning::Stall::PTR)
const PackRule<PlaneTuning::Stall> planeTuningStallPack =
    ClassPack<PlaneTuning::Stall>(
        member(float, floatPack, maxRotVel),
        member(float, floatPack, maxVel),
        member(float, floatPack, thrust),
        member(float, floatPack, damping),
        member(float, floatPack, threshold)
    );
#undef member

/**** PlaneTuning::Flight ****/
#define member(TYPE, RULE, PTR) \
  MemberRule<PlaneTuning::Flight, TYPE>(RULE, &PlaneTuning::Flight::PTR)
const PackRule<PlaneTuning::Flight> planeTuningFlightPack =
    ClassPack<PlaneTuning::Flight>(
        member(float, floatPack, maxRotVel),
        member(float, floatPack, airspeedFactor),
        member(float, floatPack, throttleInfluence),
        member(float, floatPack, throttleBreaking),
        member(float, floatPack, gravityEffect),
        member(float, floatPack, gravityEffect),
        member(float, floatPack, afterburnDrive),
        member(float, floatPack, leftoverDamping),
        member(float, floatPack, threshold)
    );
#undef member

/**** PlaneTuning ****/
#define member(TYPE, RULE, PTR) \
  MemberRule<PlaneTuning, TYPE>(RULE, &PlaneTuning::PTR)
const PackRule<PlaneTuning> planeTuningPack =
    ClassPack<PlaneTuning>(
//        member(sf::Vector2f, vectorPack, hitbox),
//        member(PlaneTuning::Energy, planeTuningEnergyPack, energy),
//        member(PlaneTuning::Stall, planeTuningStallPack, stall),
//        member(PlaneTuning::Flight, planeTuningFlightPack, flight),
//        member(float, floatPack, throttleSpeed)
    );
#undef member

}
}

#endif //SOLEMNSKY_DELTA_H
