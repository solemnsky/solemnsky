/**
 * solemnsky: the open-source multiplayer competitive 2D plane game
 * Copyright (C) 2016  Chris Gadzinski
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
/**
 * Basic state types for Plane.
 */
#pragma once
#include <bitset>
#include "util/types.hpp"
#include "sky/sky/physics.hpp"

namespace sky {

/**
 * Vectors of action to control a Participation. Passed along with a state,
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
 * The POD static state of a Plane.
 */
struct PlaneTuning {
  PlaneTuning(); // constructs with sensible defaults

  template<class Archive>
  void serialize(Archive &ar) {
    ar(energy.thrustDrain, energy.recharge, energy.laserGun,
       energy.primaryRecharge);
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
    // weapon mechanics
    float primaryRecharge;
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
 * Subset of the PlaneState a client can transmit to the server.
 */
struct PlaneStateClient {
  PlaneStateClient() = default;
  PlaneStateClient(const struct PlaneState &state);

  template<typename Archive>
  void serialize(Archive &ar) {
    ar(physical, airspeed, throttle, stalled);
  }

  PhysicalState physical;
  Clamped airspeed, throttle;
  bool stalled;

};

/**
 * Subset of the PlaneState a server can transmit to a client in authority.
 */
struct PlaneStateServer {
  PlaneStateServer() = default;
  PlaneStateServer(const struct PlaneState &state);

  template<typename Archive>
  void serialize(Archive &ar) {
    ar(energy, health, primaryCooldown);
  }

  Clamped energy, health;
  Clamped primaryCooldown;

};

/**
 * The volatile state of a plane.
 */
struct PlaneState {
  PlaneState(); // for packing
  PlaneState(const PlaneTuning &tuning,
             const sf::Vector2f &pos,
             const float rot);

  template<class Archive>
  void serialize(Archive &ar) {
    ar(physical, stalled, airspeed, afterburner, throttle, leftoverVel);
    ar(energy, health, primaryCooldown);
  }

  // State.
  PhysicalState physical;
  bool stalled;
  Clamped airspeed, afterburner, throttle;
  sf::Vector2f leftoverVel;

  Clamped energy, health;
  Cooldown primaryCooldown;

  // Utility accessors.
  float forwardVelocity() const;
  float velocity() const;

  // Applying state subsets.
  void applyClient(const PlaneStateClient &client);
  void applyServer(const PlaneStateServer &server);

};

/**
 * The persistent control state of a Participation; synchronized along with the
 * rest of the Participation's state.
 */
struct PlaneControls {
  friend bool operator==(const PlaneControls &x, const PlaneControls &y);
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

bool operator==(const PlaneControls &x, const PlaneControls &y);
bool operator!=(const PlaneControls &x, const PlaneControls &y);

}

