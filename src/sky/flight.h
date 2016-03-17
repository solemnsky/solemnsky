/**
 * The flight! Also the landing, and everything else.
 */
#pragma once
#include <Box2D/Box2D.h>
#include "physics.h"
#include "util/types.h"

namespace sky {

class Sky;

/**
 * Tuning values describing how a plane flies.
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
    // TODO: document this
    float maxRotVel = 180,
        airspeedFactor = 330,
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
 * The (POD) variable game state of a Plane.
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

  /**
   * Data.
   * Clamped values should have the same bounds in all instantiations.
   */
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

/**
 * Initializer to construct a new plane.
 */
struct PlaneInitializer {
  PlaneInitializer();
  PlaneInitializer(const PlaneTuning &tuning, const PlaneState &state);

  template<typename Archive>
  void serialize(Archive &ar) { ar(tuning, state); }

  PlaneTuning tuning;
  PlaneState state;
};

/**
 * A plane in the sky. Manager of a a Box2D body entity, it's non-copyable.
 */
class Plane {
 private:
  sky::Sky *parent;
  Physics *physics;
  b2Body *body;

  /**
   * State mutation.
   */
  friend class Sky; // Sky calls these functions

  void writeToBody();
  void readFromBody();
  void tick(float d);

 public:
  Plane(Sky *parent,
        const PlaneTuning &tuning,
        const sf::Vector2f pos,
        const float rot);
  Plane(Sky *parent, const PlaneInitializer &initializer);
  Plane(Plane &&plane);
  ~Plane();

  Plane(const Plane &) = delete;
  Plane &operator=(const Plane &) = delete;

  /**
   * State
   * `tuning` should be treated as const; it's only not defined as such to
   * allow Plane(Plane &&) to exist, which makes some matters a lot easier
   */
  PlaneTuning tuning;
  PlaneState state;

  /**
   * Initializer.
   * For most game objects we also have a delta, but for our planes the delta
   * resolution is just to copy PlaneState. TODO: make a smarter delta.
   */
  PlaneInitializer captureInitializer() const;
};

}
