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
#include "plane.hpp"
#include "util/methods.hpp"
#include "engine/player.hpp"
namespace sky {

/**
 * Plane.
 */

void Plane::switchStall() {
  const float forwardVel = state.forwardVelocity();
  if (state.stalled) {
    if (forwardVel > tuning.stall.threshold) {
      state.stalled = false;
      state.leftoverVel =
          state.physical.vel
              - (forwardVel * VecMath::fromAngle(state.physical.rot));

      state.airspeed = forwardVel / tuning.flight.airspeedFactor;
      state.throttle =
          state.airspeed / tuning.flight.throttleInfluence;
    }
  } else {
    if (forwardVel < tuning.flight.threshold) {
      state.stalled = true;
      state.throttle = 1;
      state.airspeed = 0;
    }
  }
}

void Plane::tickFlight(const TimeDiff delta) {
  switchStall();
  const float velocity = state.velocity();

  Movement throtCtrl =
      addMovement(controls.getState<Action::Reverse>(),
                  controls.getState<Action::Thrust>());
  Movement rotCtrl = controls.rotMovement();

  // set rotation
  state.physical.rotvel = ((state.stalled) ?
                           tuning.stall.maxRotVel :
                           tuning.flight.maxRotVel)
      * movementValue(rotCtrl);

  state.energy += tuning.energy.recharge * delta;
  state.afterburner = 0;

  if (state.stalled) {
    // Afterburner.
    if (throtCtrl == Movement::Up) {
      const float thrustEfficacy =
          requestEnergy(tuning.energy.thrustDrain * delta);

      state.physical.vel +=
          VecMath::fromAngle(state.physical.rot) *
              float(delta * tuning.stall.thrust * thrustEfficacy);
      state.afterburner = thrustEfficacy;
    }

    // Damping towards terminal velocity.
    float excessVel = velocity - tuning.stall.maxVel;
    float dampingFactor = tuning.stall.maxVel / velocity;
    if (excessVel > 0)
      state.physical.vel = state.physical.vel *
          float(dampingFactor * std::pow(tuning.stall.damping, delta));
  } else {
    // Modify throttle and afterburner according to controls.
    state.throttle += movementValue(throtCtrl) * delta;
    bool afterburning =
        (throtCtrl == Movement::Up) && state.throttle == 1;

    // Pick away at leftover velocity.
    state.leftoverVel *= std::pow(tuning.flight.leftoverDamping, delta);

    // Modify airspeed.
    float speedMod = 0;
    speedMod +=
        sin(toRad(state.physical.rot)) * tuning.flight.gravityEffect * delta;
    if (afterburning) {
      const float thrustEfficacity =
          requestEnergy(tuning.energy.thrustDrain * delta);
      state.afterburner = thrustEfficacity;
      speedMod += tuning.flight.afterburnDrive * delta * thrustEfficacity;
    }
    state.airspeed += speedMod;

    const float targetThrottle = state.throttle * tuning.flight.throttleInfluence,
        throttleEffectFactor =
        (state.airspeed <= tuning.flight.throttleInfluence || state.throttle < 0.9)
        ? 1 : tuning.flight.throttleGlideDamper;

    if (state.airspeed > targetThrottle) {
      approach(state.airspeed, targetThrottle,
               tuning.flight.throttleBrakeEffect * throttleEffectFactor * delta);
    } else {
      approach(state.airspeed, targetThrottle,
               tuning.flight.throttleEffect * throttleEffectFactor * delta);
    }

    float targetSpeed = state.airspeed * tuning.flight.airspeedFactor;

    // Set velocity, according to target speed, rotation, and leftoverVel.
    state.physical.vel = targetSpeed * VecMath::fromAngle(state.physical.rot) +
        state.leftoverVel;
  }
}

void Plane::tickWeapons(const TimeDiff delta) {
  state.primaryCooldown.cool(tuning.energy.primaryRecharge * delta);
}

void Plane::writeToBody() {
  state.physical.writeToBody(physics, body);
  body->SetGravityScale(state.stalled ? 1 : 0);
}

void Plane::readFromBody() {
  state.physical.readFromBody(physics, body);
}

void Plane::prePhysics() {
  writeToBody();
}

void Plane::postPhysics(const TimeDiff delta) {
  readFromBody();
  tickFlight(delta);
  tickWeapons(delta);
}

void Plane::onBeginContact(const BodyTag &body) {
  if (body.type == BodyTag::Type::PropTag) {
    if (*body.player != player) {
      state.health = 0;
    }
  }
}

void Plane::onEndContact(const BodyTag &body) {

}

Plane::Plane(Player &player,
             Physics &physics,
             const PlaneControls &controls,
             const PlaneTuning &tuning,
             const PlaneState &state) :
    physics(physics),
    controls(controls),

    tuning(tuning),
    state(state),
    body(physics.createBody(Shape::Rectangle(tuning.hitbox),
                            BodyTag::PlaneTag(*this, player), true)),
    player(player) {
  state.physical.hardWriteToBody(physics, body);
  body->SetGravityScale(state.stalled ? 1 : 0);
}

Plane::~Plane() {
  physics.deleteBody(body);
}

const PlaneTuning &Plane::getTuning() const {
  return tuning;
}

const PlaneState &Plane::getState() const {
  return state;
}

bool Plane::requestDiscreteEnergy(const float reqEnergy) {
  if (state.energy < reqEnergy) return false;
  state.energy -= reqEnergy;
  return true;
}

float Plane::requestEnergy(const float reqEnergy) {
  const float initEnergy = state.energy;
  state.energy -= reqEnergy;
  return (initEnergy - state.energy) / reqEnergy;
}

void Plane::resetPrimary() {
  state.primaryCooldown.reset();
}

void Plane::damage(const float amount) {
  state.health -= amount;
}

}
