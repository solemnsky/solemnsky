#include <cmath>
#include <SFML/Graphics/Rect.hpp>
#include "arena.h"
#include "plane.h"
#include "sky.h"
#include "util/methods.h"

namespace sky {

/**
 * Action.
 */

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
    primaryCooldown(1),
    physical(pos,
             tuning.flight.airspeedFactor * VecMath::fromAngle(rot),
             rot,
             0),

    stalled(false), afterburner(0),
    airspeed(tuning.flight.throttleInfluence),
    leftoverVel(0, 0),
    throttle(1),

    energy(1),
    health(1) { }

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
 * PlaneInitializer.
 */

PlaneInitializer::PlaneInitializer() { }

PlaneInitializer::PlaneInitializer(
    const PlaneControls &controls,
    const PlaneTuning &tuning,
    const PlaneState &state) :
    spawn(std::pair<PlaneTuning, PlaneState>(tuning, state)),
    controls(controls) { }

/**
 * PlaneDelta.
 */

PlaneDelta::PlaneDelta() { }

sky::PlaneDelta::PlaneDelta(const sky::PlaneTuning &tuning,
                            const sky::PlaneState &state,
                            const PlaneControls &controls) :
    tuning(tuning), state(state), controls(controls) { }

PlaneDelta::PlaneDelta(const PlaneState &state, const PlaneControls &controls) :
    state(state), controls(controls) { }

bool PlaneDelta::verifyStructure() const {
  return true;
//  return not (!bool(tuning) or bool(state));
}

/**
 * PlaneVital.
 */

PlaneVital::PlaneVital(Sky &parent,
                       const PlaneControls &controls,
                       const PlaneTuning &tuning,
                       const PlaneState &state) :
    parent(parent), physics(parent.physics),
    controls(controls),
    tuning(tuning), state(state),
    body(physics.rectBody(tuning.hitbox)) {
  body = physics.rectBody(tuning.hitbox);
  this->state.physical.hardWriteToBody(physics, body);
  body->SetGravityScale(state.stalled ? 1 : 0);
}

PlaneVital::~PlaneVital() {
  physics.clrBody(body);
};

void PlaneVital::writeToBody() {
  state.physical.writeToBody(physics, body);
  body->SetGravityScale(state.stalled ? 1 : 0);
}

void PlaneVital::readFromBody() {
  state.physical.readFromBody(physics, body);
}

void PlaneVital::tickFlight(const float delta) {
  // helpful synonyms
  const float
      forwardVel = state.forwardVelocity(),
      velocity = state.velocity();

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
    // afterburner
    if (throtCtrl == Movement::Up) {
      const float thrustEfficacy =
          state.requestEnergy(tuning.energy.thrustDrain * delta);

      state.physical.vel +=
          VecMath::fromAngle(state.physical.rot) *
              (delta * tuning.stall.thrust * thrustEfficacy);
      state.afterburner = thrustEfficacy;
    }

    // damping towards terminal velocity
    float excessVel = velocity - tuning.stall.maxVel;
    float dampingFactor = tuning.stall.maxVel / velocity;
    if (excessVel > 0)
      state.physical.vel = state.physical.vel * dampingFactor *
          std::pow(tuning.stall.damping, delta);
  } else {
    // modify throttle and afterburner according to controls
    state.throttle += movementValue(throtCtrl) * delta;
    bool afterburning =
        (throtCtrl == Movement::Up) && state.throttle == 1;

    // pick away at leftover velocity
    state.leftoverVel *= std::pow(tuning.flight.leftoverDamping, delta);

    // modify vstate.airspeed
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

    // set velocity, according to target speed, rotation, and leftoverVel
    state.physical.vel = targetSpeed * VecMath::fromAngle(state.physical.rot) +
        state.leftoverVel;
  }

  // stall singularities
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
void PlaneVital::tickWeapons(const float delta) {
  state.primaryCooldown.cool(delta);
  if (state.primaryCooldown
      && this->controls.getState<Action::Primary>()) {
    if (state.requestDiscreteEnergy(
        tuning.energy.laserGun)) {
// TODO: better props
//      props.emplace_front(
//          parent,
//          vital->state.physical.pos + 100.0f * VecMath::fromAngle(
//              vital->state.physical.rot),
//          vital->state.physical.vel + 300.0f * VecMath::fromAngle
//              (vital->state.physical.rot));
//      primaryCooldown.reset();
    }
  }
}

void PlaneVital::tick(const float delta) {
  tickFlight(delta);
  tickWeapons(delta);
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

/**
 * Plane.
 */

void Plane::spawnWithState(const PlaneTuning &tuning,
                           const PlaneState &state) {
  vital.emplace(parent, controls, tuning, state);
}

void Plane::beforePhysics() {
  if (vital) vital->writeToBody();
  for (auto &prop : props) prop.writeToBody();
}

void Plane::afterPhysics(float delta) {
  if (isSpawned()) {
    vital->readFromBody();
    vital->tick(delta);

    for (auto &prop : props) {
      prop.readFromBody();
      prop.tick(delta);
    }

    props.remove_if([](Prop &prop) {
      return prop.lifeTime > 1;
    });
  }
}

void Plane::spawn(const PlaneTuning &tuning,
                  const sf::Vector2f pos,
                  const float rot) {
  spawnWithState(tuning, PlaneState(tuning, pos, rot));
  newlyAlive = true;
}

void Plane::doAction(const Action action, bool actionState) {
  controls.doAction(action, actionState);
}

void Plane::applyDelta(const PlaneDelta &delta) {
  if (delta.tuning) {
    spawnWithState(delta.tuning.get(), delta.state.get());
    return;
  }
  if (delta.state) {
    if (vital) vital->state = *delta.state;
  } else vital.reset();
  controls = delta.controls;
}

PlaneInitializer Plane::captureInitializer() const {
  if (vital) {
    return PlaneInitializer(controls, vital->tuning, vital->state);
  } else {
    return {};
  }
}

PlaneDelta Plane::captureDelta() {
  if (newlyAlive) {
    newlyAlive = false;
    return PlaneDelta(vital->tuning, vital->state, controls);
  } else {
    if (vital) return PlaneDelta(vital->state, controls);
    else return PlaneDelta();
  }
}

Plane::Plane(Sky &parent, Player &player) :
    parent(parent),
    player(player),
    newlyAlive(false) { }

Plane::Plane(Sky &parent, Player &player,
             const PlaneInitializer &initializer) :
    Plane(parent, player) {
  if (initializer.spawn)
    spawnWithState(initializer.spawn->first, initializer.spawn->second);
  controls = initializer.controls;
}

bool Plane::isSpawned() const {
  return bool(vital);
}

}

