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
#include "planestate.h"
#include "util/methods.h"

/**
 * Action.
 */

namespace sky {

static const std::vector<std::string> actionNames = {
    "thrust",
    "reverse",
    "left",
    "right",
    "primary",
    "secondary",
    "special",
    "suicide"
};

std::string showAction(const Action action) {
  return actionNames.at((size_t) action);
}

optional<Action> readAction(const std::string &str) {
  Action action = Action::Thrust;
  for (const auto &search : actionNames) {
    if (search == str) return action;
    action = Action(char(action) + 1);
  }
  return {};
}

/**
 * PlaneTuning.
 */

PlaneTuning::PlaneTuning() :
    hitbox(110, 60),
    maxHealth(10),
    throttleSpeed(1.5) { }

PlaneTuning::Energy::Energy() :
    thrustDrain(1),
    recharge(0.5),
    laserGun(0.3) { }

PlaneTuning::Stall::Stall() :
    maxRotVel(200),
    maxVel(300),
    thrust(500),
    damping(0.8),
    threshold(130) { }

PlaneTuning::Flight::Flight() :
    maxRotVel(180),
    airspeedFactor(330),
    throttleInfluence(0.6),
    throttleEffect(0.3),
    gravityEffect(0.6),
    afterburnDrive(0.9),
    leftoverDamping(0.3),
    threshold(110) { }

/**
 * PlaneState.
 */

PlaneState::PlaneState() : primaryCooldown(1) { }

PlaneState::PlaneState(const PlaneTuning &tuning,
                       const sf::Vector2f &pos,
                       const float rot) :
    physical(pos,
             tuning.flight.airspeedFactor * VecMath::fromAngle(rot),
             rot, 0),

    stalled(false), afterburner(0),
    leftoverVel(0, 0),
    airspeed(tuning.flight.throttleInfluence),
    throttle(1),

    energy(1),
    health(1),
    primaryCooldown(1) { }

float PlaneState::forwardVelocity() const {
  return velocity() *
      (const float) cos(
          toRad(physical.rot) - std::atan2(physical.vel.y, physical.vel.x));
}

float PlaneState::velocity() const {
  return VecMath::length(physical.vel);
}

bool PlaneState::requestDiscreteEnergy(const float reqEnergy) {
  if (energy < reqEnergy) return false;
  energy -= reqEnergy;
  return true;
}

float PlaneState::requestEnergy(const float reqEnergy) {
  const float initEnergy = energy;
  energy -= reqEnergy;
  return (initEnergy - energy) / reqEnergy;
}

/**
 * PlaneControls.
 */

PlaneControls::PlaneControls() : controls(0) { }

void PlaneControls::doAction(const Action action, const bool actionState) {
  controls[size_t(action)] = actionState;
}

bool PlaneControls::getState(const Action action) const {
  return controls[size_t(action)];
}

Movement PlaneControls::rotMovement() const {
  return addMovement(getState<Action::Left>(),
                     getState<Action::Right>());
}

bool operator==(const PlaneControls &x, const PlaneControls &y) {
  return x.controls == y.controls;
}

bool operator!=(const PlaneControls &x, const PlaneControls &y) {
  return !(x == y);
}

}

