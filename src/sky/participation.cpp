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
#include <cmath>
#include <SFML/Graphics/Rect.hpp>
#include "arena.h"
#include "participation.h"
#include "sky.h"
#include "util/printer.h"

namespace sky {


/**
 * ParticipationInit.
 */

ParticipationInit::ParticipationInit() { }

ParticipationInit::ParticipationInit(
    const PlaneControls &controls,
    const PlaneTuning &tuning,
    const PlaneState &state) :
    spawn(std::pair<PlaneTuning, PlaneState>(tuning, state)),
    controls(controls) { }

ParticipationInit::ParticipationInit(
    const PlaneControls &controls) : controls(controls) { }

/**
 * ParticipationDelta.
 */

bool ParticipationDelta::verifyStructure() const {
  return imply(bool(spawn), bool(state));
}

ParticipationDelta ParticipationDelta::respectClientAuthority() const {
  ParticipationDelta delta;
  delta.planeAlive = planeAlive;
  delta.spawn = spawn;
  return delta;
}

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
          state.requestEnergy(tuning.energy.thrustDrain * delta);

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
          state.requestEnergy(tuning.energy.thrustDrain * delta);
      state.afterburner = thrustEfficacity;
      speedMod += tuning.flight.afterburnDrive * delta * thrustEfficacity;
    }
    state.airspeed += speedMod;

    approach(state.airspeed,
             (float) state.throttle * tuning.flight.throttleInfluence,
             tuning.flight.throttleEffect * delta);

    float targetSpeed = state.airspeed * tuning.flight.airspeedFactor;

    // Set velocity, according to target speed, rotation, and leftoverVel.
    state.physical.vel = targetSpeed * VecMath::fromAngle(state.physical.rot) +
        state.leftoverVel;
  }
}

void Plane::tickWeapons(const TimeDiff delta) {
  state.primaryCooldown.cool(delta);
  if (state.primaryCooldown
      && this->controls.getState<Action::Primary>()) {
    if (state.requestDiscreteEnergy(
        tuning.energy.laserGun)) {
      state.primaryCooldown.reset();
      appLog("implement pewpew plz");
// TODO: better props
//      props.emplace_front(
//          parent,
//          participation->state.physical.pos + 100.0f * VecMath::fromAngle(
//              participation->state.physical.rot),
//          participation->state.physical.vel + 300.0f * VecMath::fromAngle
//              (participation->state.physical.rot));
//      primaryCooldown.reset();
    }
  }
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

}

void Plane::onEndContact(const BodyTag &body) {

}

void Plane::applyInput(const ParticipationInput &input) {
  if (input.physical)
    state.physical = input.physical.get();
}

Plane::Plane(Physics &physics,
             const PlaneControls &controls,
             const PlaneTuning &tuning,
             const PlaneState &state) :
    physics(physics),
    controls(controls),

    tuning(tuning),
    state(state),
    body(physics.createBody(physics.rectShape(tuning.hitbox),
                            BodyTag::PlaneTag(*this))) {
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

/**
 * Participation.
 */

void Participation::spawnWithState(const PlaneTuning &tuning,
                                   const PlaneState &state) {
  plane.emplace(physics, controls, tuning, state);
}

void Participation::doAction(const Action action, bool actionState) {
  controls.doAction(action, actionState);
  if (action == Action::Suicide && actionState) {
    plane.reset();
  }
}

void Participation::prePhysics() {
  if (plane) plane->prePhysics();
  for (auto &prop : props) prop.writeToBody();
}

void Participation::postPhysics(const float delta) {
  if (plane) plane->postPhysics(delta);

  for (auto &prop : props) {
    prop.readFromBody();
    prop.tick(delta);
  }

  props.remove_if([](Prop &prop) {
    return prop.lifetime > 1;
  });
}

void Participation::spawn(const PlaneTuning &tuning,
                          const sf::Vector2f &pos,
                          const float rot) {
  spawnWithState(tuning, PlaneState(tuning, pos, rot));
  newlyAlive = true;
}

Participation::Participation(Physics &physics,
                             const ParticipationInit &initializer) :
    Networked(initializer),
    physics(physics),
    newlyAlive(false) {
  if (initializer.spawn)
    spawnWithState(initializer.spawn->first, initializer.spawn->second);
  controls = initializer.controls;
}

void Participation::applyDelta(const ParticipationDelta &delta) {
  if (delta.spawn) {
    spawnWithState(delta.spawn->first, delta.spawn->second);
  } else {
    if (delta.planeAlive) {
      if (plane and delta.state) plane->state = *delta.state;
    } else plane.reset();
  }

  if (delta.controls) {
    controls = *delta.controls;
  }
}

ParticipationInit Participation::captureInitializer() const {
  if (plane) {
    return ParticipationInit(controls, plane->tuning, plane->state);
  } else {
    return ParticipationInit(controls);
  }
}

ParticipationDelta Participation::collectDelta() {
  ParticipationDelta delta;

  delta.planeAlive = bool(plane);
  if (plane) {
    if (newlyAlive) {
      delta.spawn.emplace(plane->tuning, plane->state);
      newlyAlive = false;
    } else {
      delta.state.emplace(plane->state);
    }
  }

  delta.controls = controls;
  return delta;
}

void Participation::applyInput(const ParticipationInput &input) {
  if (input.controls) {
    controls = input.controls.get();
  }

  if (plane) {
    plane->applyInput(input);
  }
}

const optional<Plane> &Participation::getPlane() const {
  return plane;
}

const std::forward_list<Prop> &Participation::getProps() const {
  return props;
}

const PlaneControls &Participation::getControls() const {
  return controls;
}

bool Participation::isSpawned() const {
  return bool(plane);
}

}

