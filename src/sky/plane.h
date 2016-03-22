/**
 * The flight! Also the landing, and everything else.
 */
#pragma once
#include <Box2D/Box2D.h>
#include "action.h"
#include "physics.h"
#include "util/types.h"

namespace sky {

class Sky;

/**
 * The POD static state of a PlaneVital.
 */
struct PlaneTuning {
  PlaneTuning(); // constructs with sensible defaults

  template<class Archive>
  void serialize(Archive &ar) {
    ar(energy.thrustDrain, energy.recharge, energy.laserGun);
    ar(stall.maxRotVel, stall.maxVel, stall.thrust, stall.damping);
    ar(flight.maxRotVel, flight.airspeedFactor, flight.throttleInfluence,
       flight.throttleEffect, flight.gravityEffect, flight.afterburnDrive,
       flight.leftoverDamping, flight.threshold, throttleSpeed);
  }

  sf::Vector2f hitbox{110, 60}; // x axis parallel with flight direction
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
    float maxRotVel = 180, // how quickly we can turn
        airspeedFactor = 330, //
        throttleInfluence = 0.6,
        throttleEffect = 0.3,
        gravityEffect = 0.6,
        afterburnDrive = 0.9,
        leftoverDamping = 0.3;
    float threshold = 110; // the maximum airspeed that we need to enter stall
  } flight;

  float throttleSpeed = 1.5;
};

/**
 * The POD variable game state of a PlaneVital.
 */
struct PlaneState {
  PlaneState(); // for packing
  PlaneState(const PlaneTuning &tuning,
             const sf::Vector2f &pos,
             const float rot);

  template<class Archive>
  void serialize(Archive &ar) {
    ar(rotCtrl, throtCtrl, pos, vel, rot, rotvel, stalled, afterburner,
       leftoverVel, airspeed, throttle, energy, health);
  }

  Movement rotCtrl; // controls
  Movement throtCtrl;

  sf::Vector2f pos, vel; // physical values
  Angle rot;
  float rotvel;

  bool stalled; // flight mechanics
  Clamped afterburner;
  sf::Vector2f leftoverVel;
  Clamped airspeed, throttle;

  Clamped energy, health; // game mechanics

  float forwardVelocity() const;
  float velocity() const;

  // returns true if energy was drawn
  bool requestDiscreteEnergy(const float reqEnergy);
  // returns the fraction of the requested energy that was drawn
  float requestEnergy(const float reqEnergy);
};

/**
 * Initializer to copy a Plane over a network.
 */
struct PlaneInitializer: public VerifyStructure {
  PlaneInitializer();
  PlaneInitializer(const PlaneTuning &tuning, const PlaneState &state);

  template<typename Archive>
  void serialize(Archive &ar) { ar(tuning, state); }

  bool verifyStructure() const;

  optional<PlaneTuning> tuning;
  optional<PlaneState> state;
};

/**
 * Delta in a Plane to sync a Plane over a network..
 */
struct PlaneDelta: public VerifyStructure {
  PlaneDelta();
  PlaneDelta(const PlaneTuning &tuning, const PlaneState &state);
  PlaneDelta(const PlaneState &state);

  template<typename Archive>
  void serialize(Archive &ar) {
    ar(tuning, state);
  }

  bool verifyStructure() const;

  optional<PlaneTuning> tuning;
  optional<PlaneState> state;
};

/**
 * The state and Box2D allocation of a plane when alive.
 */
class PlaneVital {
 private:
  sky::Sky *parent;
  Physics *physics;
  b2Body *body;

 public:
  PlaneVital(Sky *parent,
             const PlaneTuning &tuning,
             const PlaneState &state);
  PlaneVital(PlaneVital &&plane);
  ~PlaneVital();

  PlaneVital(const PlaneVital &) = delete;
  PlaneVital &operator=(const PlaneVital &) = delete;

  PlaneTuning tuning;
  PlaneState state;

  void writeToBody();
  void readFromBody();
  void tick(float d);

};

/**
 * Top-level entity representing a Player's participation in the Sky. Can be
 * alive or not.
 */
class Plane {
 private:
  optional<PlaneVital> vital;

 public:
  Plane(Sky *parent);
  Plane(Sky *parent, const PlaneInitializer &initializer);

  /**
   * API for user.
   */
  void spawn(const PlaneTuning &tuning,
             const sf::Vector2f pos,
             const float rot);
  void doAction(const Action &action);

  void applyDelta(const PlaneDelta &delta);
  PlaneInitializer captureInitializer() const;
  PlaneDelta captureDelta();
};

}
