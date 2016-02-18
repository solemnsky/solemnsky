/**
 * Data structures / simulation and box2d integration for planes.
 */
#ifndef SOLEMNSKY_FLIGHT_H
#define SOLEMNSKY_FLIGHT_H

#include <Box2D/Box2D.h>
#include "physics.h"
#include "util/types.h"
#include "telegraph/pack.h"

namespace sky {

class Sky;

/**
 * Tuning values describing how a plane flies.
 */
struct PlaneTuning {
  PlaneTuning() { }

  sf::Vector2f hitbox{110, 60}; // x axis parallel with flight
  float maxHealth = 10;

  struct Energy {
    float thrustDrain = 1;
    float recharge = 0.5;
    float laserGun = 0.3;
  } energy;

  struct Stall {
    // mechanics when stalled
    float maxRotVel = 200, // how quickly we can turn, (deg / s)
        maxVel = 300, // our terminal velocity (px / s)
        thrust = 500, // thrust acceleration (ps / s^2)
        damping = 0.8; // how quickly we approach our terminal velocity
    float threshold = 130; // the minimum airspeed that we need to enter flight
  } stall;

  struct Flight {
    // mechanics when not stalled
    float maxRotVel = 180,
        airspeedFactor = 330,
        throttleInfluence = 0.6,
        throttleDrive = 0.3,
        throttleBreaking = 1.1,
        gravityEffect = 0.6,
        afterburnDrive = 0.9,
        leftoverDamping = 0.3;
    float threshold = 110; // the maximum airspeed that we need to enter stall
  } flight;

  float throttleSpeed = 1.5;
};

#define member(TYPE, PTR, RULE) \
  tg::MemberRule<PlaneTuning::Energy, TYPE>(RULE, &PlaneTuning::Energy::PTR)
const static tg::Pack<PlaneTuning::Energy> planeTuningEnergyPack =
    tg::ClassPack<PlaneTuning::Energy>(
        member(float, thrustDrain, tg::floatPack),
        member(float, recharge, tg::floatPack),
        member(float, laserGun, tg::floatPack)
    );
#undef member

#define member(TYPE, PTR, RULE) \
  tg::MemberRule<PlaneTuning::Stall, TYPE>(RULE, &PlaneTuning::Stall::PTR)
const static tg::Pack<PlaneTuning::Stall> planeTuningStallPack =
    tg::ClassPack<PlaneTuning::Stall>(
        member(float, maxRotVel, tg::floatPack),
        member(float, maxVel, tg::floatPack),
        member(float, thrust, tg::floatPack),
        member(float, damping, tg::floatPack),
        member(float, threshold, tg::floatPack)
    );
#undef member

#define member(TYPE, PTR, RULE) \
  tg::MemberRule<PlaneTuning::Flight, TYPE>(RULE, &PlaneTuning::Flight::PTR)
const static tg::Pack<PlaneTuning::Flight> planeTuningFlightPack =
    tg::ClassPack<PlaneTuning::Flight>(
        member(float, maxRotVel, tg::floatPack),
        member(float, airspeedFactor, tg::floatPack),
        member(float, throttleInfluence, tg::floatPack),
        member(float, throttleBreaking, tg::floatPack),
        member(float, gravityEffect, tg::floatPack),
        member(float, gravityEffect, tg::floatPack),
        member(float, afterburnDrive, tg::floatPack),
        member(float, leftoverDamping, tg::floatPack),
        member(float, threshold, tg::floatPack)
    );
#undef member

#define member(TYPE, PTR, RULE) \
  tg::MemberRule<PlaneTuning, TYPE>(RULE, &PlaneTuning::PTR)
const static tg::Pack<PlaneTuning> planeTuningPack =
    tg::ClassPack<PlaneTuning>(
        member(sf::Vector2f, hitbox, tg::vectorPack),
        member(PlaneTuning::Energy, energy, planeTuningEnergyPack),
        member(PlaneTuning::Stall, stall, planeTuningStallPack),
        member(PlaneTuning::Flight, flight, planeTuningFlightPack),
        member(float, throttleSpeed, tg::floatPack)
    );
#undef member

/**
 * State specific to a plane that is spawned, everything in here is expected
 * to change very frequently, while it exists anyway.
 */
struct PlaneVital {
  PlaneVital(); // for packing
  PlaneVital(const PlaneTuning &tuning,
             const sf::Vector2f &pos,
             const float rot);

  /**
   * Data.
   * *** ALL float encapsulations from util should have equal parameters in
   * all instances of PlaneVital. This is essential to serialization and
   * sanity in general. ****
   */
  Clamped rotCtrl; // controls
  Movement throtCtrl;

  sf::Vector2f pos, vel; // physical values
  Angle rot;
  float rotvel;

  bool stalled; // flight mechanics
  Clamped afterburner;
  sf::Vector2f leftoverVel;
  Clamped airspeed, throttle;

  Clamped energy, health; // game mechanics

  /**
   * Helper methods.
   */
  float forwardVelocity() const;
  float velocity() const;

  // returns true if energy was drawn
  bool requestDiscreteEnergy(const float reqEnergy);
  // returns the fraction of the requested energy that was drawn
  float requestEnergy(const float reqEnergy);
};

#define member(TYPE, PTR, RULE) \
  tg::MemberRule<PlaneVital, TYPE>(RULE, &PlaneVital::PTR)
const static tg::Pack<PlaneVital> planeVitalPack =
    tg::ClassPack<PlaneVital>(
        member(Clamped, rotCtrl, tg::clampedPack),
        member(Movement, throtCtrl, tg::movementPack),
        member(sf::Vector2f, pos, tg::vectorPack),
        member(sf::Vector2f, vel, tg::vectorPack),
        member(Angle, rot, tg::anglePack),
        member(float, rotvel, tg::floatPack),
        member(bool, stalled, tg::boolPack),
        member(Clamped, afterburner, tg::clampedPack),
        member(sf::Vector2f, leftoverVel, tg::vectorPack),
        member(Clamped, airspeed, tg::clampedPack),
        member(Clamped, throttle, tg::clampedPack),
        member(Clamped, energy, tg::clampedPack),
        member(Clamped, health, tg::clampedPack)
    );
#undef member

/**
 * A plane, expressed in a simple (copyable etc.) struct.
 */
struct Plane {
  Plane() = default;

  /**
   * Data.
   */
  PlaneTuning tuning;
  optional<PlaneVital> vital; // exists <=> plane is spawned
};

#define member(TYPE, PTR, RULE) \
  tg::MemberRule<Plane, TYPE>(RULE, &Plane::PTR)
const static tg::Pack<Plane> planePack =
    tg::ClassPack<Plane>(
        member(PlaneTuning, tuning, planeTuningPack),
        member(optional<PlaneVital>, vital,
               tg::OptionalPack<PlaneVital>(planeVitalPack))
    );
#undef member

/**
 * Handle for a plane, holds
 */
class PlaneHandle {
private:
  sky::Sky *engine;
  Physics *physics;
  b2Body *body;

  /*
   * State mutation.
   */
  friend class Sky;

  void writeToBody();
  void readFromBody();
  void tick(float d);

  void spawn(const PlaneTuning &tuning,
             const sf::Vector2f pos,
             const float rot);
  void kill();

public:
  PlaneHandle(Sky *engine);
  ~PlaneHandle();

  PlaneHandle(PlaneHandle &&);
  PlaneHandle(const PlaneHandle &) = delete;
  PlaneHandle &operator=(const PlaneHandle &) = delete;

  Plane state{};
};
}

#endif //SOLEMNSKY_FLIGHT_H
