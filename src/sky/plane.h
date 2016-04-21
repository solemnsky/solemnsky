/**
 * The state and logic of Planes.
 */
#pragma once
#include <Box2D/Box2D.h>
#include <bitset>
#include <forward_list>
#include "prop.h"
#include "physics.h"
#include "util/types.h"

namespace sky {

class Sky;

/**
 * Vectors of action to control a Plane. Passed along with a state,
 * corresponding to whether the action was begun or ended.
 */
enum class Action {
  Thrust,
  Reverse,
  Left,
  Right,
  Primary,
  Secondary,
  Special,
  Suicide,
  MAX
};

std::string showAction(const Action action);
optional<Action> readAction(const std::string &string);

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

  sf::Vector2f hitbox; // x axis parallel with flight direction
  float maxHealth, throttleSpeed;

  struct Energy {
    Energy();
    // energy mechanics
    float thrustDrain, recharge, laserGun;
  } energy;

  struct Stall {
    Stall();
    // mechanics when stalled
    float maxRotVel, // how quickly we can turn, (deg / s)
        maxVel, // our terminal velocity (px / s)
        thrust, // thrust acceleration (ps / s^2)
        damping; // how quickly we approach our terminal velocity
    float threshold; // the minimum airspeed that we need to enter flight
  } stall;

  struct Flight {
    Flight();
    // mechanics when not stalled
    float maxRotVel = 180, // how quickly we can turn
        airspeedFactor,
        throttleInfluence,
        throttleEffect,
        gravityEffect,
        afterburnDrive,
        leftoverDamping;
    float threshold; // the maximum airspeed that we need to enter stall
  } flight;

};

/**
 * The POD variable game state of a PlaneVital, that's necessary to sync over
 * the network.
 */
struct PlaneState {
  PlaneState(); // for packing
  PlaneState(const PlaneTuning &tuning,
             const sf::Vector2f &pos,
             const float rot);

  template<class Archive>
  void serialize(Archive &ar) {
    ar(physical);
    ar(stalled, afterburner, leftoverVel, airspeed, throttle);
    ar(energy, health, primaryCooldown);
  }

  /**
   * State.
   */
  // physical state
  PhysicalState physical;
  // flight mechanics
  bool stalled;
  Clamped afterburner;
  sf::Vector2f leftoverVel;
  Clamped airspeed, throttle;
  // game mechanics
  Clamped energy, health;
  Cooldown primaryCooldown;

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
 * The persistent control state of a Plane; synchronized along with the
 * rest of the Plane's state.
 */
struct PlaneControls {
 private:
  std::bitset<size_t(Action::MAX)> controls;

 public:
  PlaneControls();

  template<typename Archive>
  void serialize(Archive &ar) {
    bool x;
    for (size_t i = 0; i < size_t(Action::MAX); ++i) {
      x = controls[i];
      ar(x);
      controls[i] = x;
      // hahahah
    }
  }

  void doAction(const Action action, const bool actionState);
  bool getState(const Action action) const;

  template<Action action>
  bool getState() const { return controls[size_t(action)]; }

  Movement rotMovement() const;
};


/**
 * Initializer to copy a Plane over a network.
 */
struct PlaneInitializer {
  PlaneInitializer();
  PlaneInitializer(const PlaneControls &controls,
                   const PlaneTuning &tuning,
                   const PlaneState &state);

  template<typename Archive>
  void serialize(Archive &ar) {
    ar(spawn, controls);
  }

  optional<std::pair<PlaneTuning, PlaneState>> spawn;
  PlaneControls controls;
};

/**
 * Delta in a Plane to sync a Plane over a network..
 * TODO: make smarter
 */
struct PlaneDelta: public VerifyStructure {
  PlaneDelta();
  PlaneDelta(const PlaneTuning &tuning, const PlaneState &state,
             const PlaneControls &controls);
  PlaneDelta(const PlaneState &state, const PlaneControls &controls);

  template<typename Archive>
  void serialize(Archive &ar) {
    ar(tuning, state, controls);
  }

  bool verifyStructure() const;

  optional<PlaneTuning> tuning;
  optional<PlaneState> state;
  PlaneControls controls;
};

/**
 * The state and Box2D allocation of a plane when alive, implementation
 * detail of Plane.
 */
class PlaneVital {
 private:
  Sky &parent;
  Physics &physics;
  b2Body *body;

  const PlaneControls &controls;

  void tickFlight(const float delta);
  void tickWeapons(const float delta);

 public:
  PlaneVital() = delete;
  PlaneVital(Sky &, PlaneControls &&, const PlaneTuning &,
             const PlaneState &) = delete; // `controls` must not be a temp
  PlaneVital(Sky &parent,
             const PlaneControls &controls,
             const PlaneTuning &tuning,
             const PlaneState &state);
  ~PlaneVital();

  PlaneVital(const PlaneVital &) = delete;
  PlaneVital &operator=(const PlaneVital &) = delete;

  PlaneTuning tuning;
  PlaneState state;

  void writeToBody();
  void readFromBody();
  void tick(const float delta);
};

/**
 * Top-level entity representing a Player's participation in the Sky. Can be
 * alive or dead. Corresponds to a Player.
 */
class Plane {
 private:
  Sky &parent;
  class Player &player; // associated player

  // For capturing deltas.
  bool newlyAlive;

  friend class Sky;

  // Internal helpers.
  void spawnWithState(const PlaneTuning &tuning, const PlaneState &state);

  /**
   * API for Sky.
   */
  void beforePhysics();
  void afterPhysics(float delta);

  void spawn(const PlaneTuning &tuning,
             const sf::Vector2f pos,
             const float rot);
  void doAction(const Action action, bool state);
  void reset();

  void applyDelta(const PlaneDelta &delta);
  PlaneInitializer captureInitializer() const;
  PlaneDelta captureDelta();

 public:
  Plane() = delete;
  Plane(Sky &parent, class Player &player);
  Plane(Sky &parent, class Player &player,
        const PlaneInitializer &initializer);

  /**
   * Top-level API.
   */
  optional<PlaneVital> vital;
  PlaneControls controls;
  std::forward_list<Prop> props;

  bool isSpawned() const;

};

}
