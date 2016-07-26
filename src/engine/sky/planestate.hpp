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
#include "physics.hpp"
#include "engine/types.hpp"

namespace sky {

void forSkyActions(std::function<void(const Action)> fn);

std::string showAction(const Action action);
optional<Action> readAction(const std::string &string);

/**
 * The POD static state of a Plane.
 */
struct PlaneTuning {
  PlaneTuning(); // constructs with sensible defaults

  template<class Archive>
  void serialize(Archive &ar) {
    ar(
        cereal::make_nvp("hitbox", hitbox),
        cereal::make_nvp("maxHealth", maxHealth),
        cereal::make_nvp("throttleSpeed", throttleSpeed),
        cereal::make_nvp("energy", energy),
        cereal::make_nvp("stall", stall),
        cereal::make_nvp("flight", flight));
  }

  sf::Vector2f hitbox; // x axis parallel with flight direction
  float maxHealth, throttleSpeed;

  struct Energy {
    Energy();
    // energy mechanics
    float thrustDrain, recharge, laserGun;
    // weapon mechanics
    float primaryRecharge;

    template<typename Archive>
    void serialize(Archive &ar) {
      ar(cereal::make_nvp("thrustDrain", thrustDrain),
         cereal::make_nvp("recharge", recharge),
         cereal::make_nvp("laserGun", laserGun),
         cereal::make_nvp("primaryRecharge", primaryRecharge));
    }
  } energy;

  struct Stall {
    Stall();
    // mechanics when stalled
    float maxRotVel, // how quickly we can turn, (deg / s)
        maxVel, // our terminal velocity (px / s)
        thrust, // thrust acceleration (ps / s^2)
        damping; // how quickly we approach our terminal velocity
    float threshold; // the minimum airspeed that we need to enter flight

    template<typename Archive>
    void serialize(Archive &ar) {
      ar(cereal::make_nvp("maxRotVel", maxRotVel),
         cereal::make_nvp("maxVel", maxVel),
         cereal::make_nvp("thrust", thrust),
         cereal::make_nvp("damping", damping),
         cereal::make_nvp("threshold", threshold));
    }
  } stall;

  struct Flight {
    Flight();
    // mechanics when not stalled
    float maxRotVel, // how quickly we can turn
        airspeedFactor,
        throttleInfluence,
        throttleEffect,
        throttleBrakeEffect,
        throttleGlideDamper,
        gravityEffect,
        afterburnDrive,
        leftoverDamping;
    float threshold; // the maximum airspeed that we need to enter stall

    template<typename Archive>
    void serialize(Archive &ar) {
      ar(cereal::make_nvp("maxRotVel", maxRotVel),
         cereal::make_nvp("airspeedFactor", airspeedFactor),
         cereal::make_nvp("throttleInfluence", throttleInfluence),
         cereal::make_nvp("throttleEffect", throttleEffect),
         cereal::make_nvp("throttleBrakeEffect", throttleBrakeEffect),
         cereal::make_nvp("throttleGlideDamper", throttleGlideDamper),
         cereal::make_nvp("gravityEffect", gravityEffect),
         cereal::make_nvp("afterburnDrive", afterburnDrive),
         cereal::make_nvp("leftoverDamping", leftoverDamping),
         cereal::make_nvp("threshold", threshold));
    }
  } flight;

  float *accessParamByName(const std::string &);
  std::string toString() const;

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

