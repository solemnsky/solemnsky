/**
 * Basic state types for Plane.
 */
#pragma once
#include "util/types.h"

/**
 * Vectors of action to control a SkyPlayer. Passed along with a state,
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
 * The POD static state of a GamePlane.
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
 * The POD variable game state of a GamePlane, that's necessary to sync over
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
  PhysicalState physical;
  bool stalled;
  Clamped afterburner;
  sf::Vector2f leftoverVel;
  Clamped airspeed, throttle;

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
 * The persistent control state of a SkyPlayer; synchronized along with the
 * rest of the SkyPlayer's state.
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

