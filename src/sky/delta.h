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

/**** float bool ****/
const Pack<float> floatPack = BytePack<float>();
const Pack<bool> boolPack = BoolPack();

/**** sf::Vector2f ****/
const tg::Pack<sf::Vector2f> vectorPack =
    ClassPack<sf::Vector2f>(
        MemberRule<sf::Vector2f, float>(floatPack, &sf::Vector2f::x),
        MemberRule<sf::Vector2f, float>(floatPack, &sf::Vector2f::y)
    );

/**** PlaneTuning::Energy ****/
#define member(TYPE, PTR, RULE) \
  MemberRule<PlaneTuning::Energy, TYPE>(RULE, &PlaneTuning::Energy::PTR)
const Pack<PlaneTuning::Energy> planeTuningEnergyPack =
    ClassPack<PlaneTuning::Energy>(
        member(float, thrustDrain, floatPack),
        member(float, recharge, floatPack),
        member(float, laserGun, floatPack)
    );
#undef member

/**** PlaneTuning::Stall ****/
#define member(TYPE, PTR, RULE) \
  MemberRule<PlaneTuning::Stall, TYPE>(RULE, &PlaneTuning::Stall::PTR)
const Pack<PlaneTuning::Stall> planeTuningStallPack =
    ClassPack<PlaneTuning::Stall>(
        member(float, maxRotVel, floatPack),
        member(float, maxVel, floatPack),
        member(float, thrust, floatPack),
        member(float, damping, floatPack),
        member(float, threshold, floatPack)
    );
#undef member

/**** PlaneTuning::Flight ****/
#define member(TYPE, PTR, RULE) \
  MemberRule<PlaneTuning::Flight, TYPE>(RULE, &PlaneTuning::Flight::PTR)
const Pack<PlaneTuning::Flight> planeTuningFlightPack =
    ClassPack<PlaneTuning::Flight>(
        member(float, maxRotVel, floatPack),
        member(float, airspeedFactor, floatPack),
        member(float, throttleInfluence, floatPack),
        member(float, throttleBreaking, floatPack),
        member(float, gravityEffect, floatPack),
        member(float, gravityEffect, floatPack),
        member(float, afterburnDrive, floatPack),
        member(float, leftoverDamping, floatPack),
        member(float, threshold, floatPack)
    );
#undef member

/**** PlaneTuning ****/
#define member(TYPE, PTR, RULE) \
  MemberRule<PlaneTuning, TYPE>(RULE, &PlaneTuning::PTR)
const Pack<PlaneTuning> planeTuningPack =
    ClassPack<PlaneTuning>(
        member(sf::Vector2f, hitbox, vectorPack),
        member(PlaneTuning::Energy, energy, planeTuningEnergyPack),
        member(PlaneTuning::Stall, stall, planeTuningStallPack),
        member(PlaneTuning::Flight, flight, planeTuningFlightPack),
        member(float, throttleSpeed, floatPack)
    );
#undef member

/**** Clamped Switched Angle ****/
const Pack<Clamped> clampedPack = AssignPack<float, Clamped>(floatPack);
const Pack<Switched> switchedPack = AssignPack<float, Switched>(floatPack);
const Pack<Angle> anglePack = AssignPack<float, Angle>(floatPack);

/**** PlaneVital ****/
#define member(TYPE, PTR, RULE) \
  MemberRule<PlaneVital, TYPE>(RULE, &PlaneVital::PTR)
const Pack<PlaneVital> planeVitalPack =
    ClassPack<PlaneVital>(
        member(Clamped, rotCtrl, clampedPack),
        member(Switched, throtCtrl, switchedPack),
        member(sf::Vector2f, pos, vectorPack),
        member(sf::Vector2f, vel, vectorPack),
        member(Angle, rot, anglePack),
        member(float, rotvel, floatPack),
        member(bool, stalled, boolPack),
        member(Clamped, afterburner, clampedPack),
        member(sf::Vector2f, leftoverVel, vectorPack),
        member(Clamped, airspeed, clampedPack),
        member(Clamped, throttle, clampedPack),
        member(Clamped, energy, clampedPack),
        member(Clamped, health, clampedPack)
    );
#undef member
}

}

#endif //SOLEMNSKY_DELTA_H
