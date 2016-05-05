#include <cmath>
#include <SFML/Graphics/Rect.hpp>
#include "arena.h"
#include "plane.h"
#include "sky.h"
#include "util/printer.h"

namespace sky {


/**
 * SkyPlayerInit.
 */

SkyPlayerInit::SkyPlayerInit() { }

SkyPlayerInit::SkyPlayerInit(
    const PlaneControls &controls,
    const PlaneTuning &tuning,
    const PlaneState &state) :
    spawn(std::pair<PlaneTuning, PlaneState>(tuning, state)),
    controls(controls) { }

SkyPlayerInit::SkyPlayerInit(
    const PlaneControls &controls) : controls(controls) { }

/**
 * SkyPlayerDelta.
 */

SkyPlayerDelta::SkyPlayerDelta() { }

bool SkyPlayerDelta::verifyStructure() const {
  return imply(bool(tuning), bool(state));
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

void Plane::tickFlight(const float delta) {
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
              (delta * tuning.stall.thrust * thrustEfficacy);
      state.afterburner = thrustEfficacy;
    }

    // Damping towards terminal velocity.
    float excessVel = velocity - tuning.stall.maxVel;
    float dampingFactor = tuning.stall.maxVel / velocity;
    if (excessVel > 0)
      state.physical.vel = state.physical.vel * dampingFactor *
          std::pow(tuning.stall.damping, delta);
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

void Plane::tickWeapons(const float delta) {
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

void Plane::postPhysics(const float delta) {
  readFromBody();
  tickFlight(delta);
  tickWeapons(delta);
}

void Plane::onBeginContact(const BodyTag &body) {
  appLog("ouch");
}

void Plane::onEndContact(const BodyTag &body) {
  appLog("that's better");
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

/**
 * SkyPlayer.
 */

void SkyPlayer::doAction(const Action action, bool actionState) {
  controls.doAction(action, actionState);
  if (action == Action::Suicide && actionState) {
    plane.reset();
  }
}

void SkyPlayer::prePhysics() {
  if (plane) plane->prePhysics();
  for (auto &prop : props) prop.writeToBody();
}

void SkyPlayer::postPhysics(const float delta) {
  if (plane) plane->postPhysics(delta);

  for (auto &prop : props) {
    prop.readFromBody();
    prop.tick(delta);
  }

  props.remove_if([](Prop &prop) {
    return prop.lifetime > 1;
  });
}

SkyPlayer::SkyPlayer(Physics &physics, const SkyPlayerInit &initializer) :
    Networked(initializer),
    physics(physics),
    newlyAlive(false) {
  if (initializer.spawn)
    spawnWithState(initializer.spawn->first, initializer.spawn->second);
  controls = initializer.controls;
}

void SkyPlayer::applyDelta(const SkyPlayerDelta &delta) {
  if (delta.tuning) {
    spawnWithState(delta.tuning.get(), delta.state.get());
  } else {
    if (delta.state) {
      if (plane) plane->state = *delta.state;
    } else plane.reset();
  }
  if (delta.controls) {
    controls = *delta.controls;
  }
}

SkyPlayerInit SkyPlayer::captureInitializer() const {
  if (plane) {
    return SkyPlayerInit(controls, plane->tuning, plane->state);
  } else {
    return SkyPlayerInit(controls);
  }
}

SkyPlayerDelta SkyPlayer::captureDelta() {
  SkyPlayerDelta delta;
  if (plane) {
    delta.state = plane->state;
    if (newlyAlive) delta.tuning = plane->tuning;
    newlyAlive = false;
  }
  delta.controls = controls;
  return delta;
}

void SkyPlayer::spawnWithState(const PlaneTuning &tuning,
                               const PlaneState &state) {
  plane.emplace(physics, controls, tuning, state);
}

void SkyPlayer::spawn(const PlaneTuning &tuning,
                      const sf::Vector2f &pos,
                      const float rot) {
  spawnWithState(tuning, PlaneState(tuning, pos, rot));
  newlyAlive = true;
}

const optional<Plane> &SkyPlayer::getPlane() const {
  return plane;
}

const std::forward_list<Prop> &SkyPlayer::getProps() const {
  return props;
}

const PlaneControls &SkyPlayer::getControls() const {
  return controls;
}

bool SkyPlayer::isSpawned() const {
  return bool(plane);
}

}

