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

PlaneState::PlaneState() { }

PlaneState::PlaneState(const PlaneTuning &tuning,
                       const sf::Vector2f &pos,
                       const float rot) :
    rotCtrl(Movement::None),
    throtCtrl(Movement::None),

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
    const PlaneTuning &tuning, const PlaneState &state) :
    spawn(std::pair<PlaneTuning, PlaneState>(tuning, state)) { }

/**
 * PlaneDelta.
 */

PlaneDelta::PlaneDelta() { }

PlaneDelta::PlaneDelta(const PlaneState &state) :
    state(state) { }

sky::PlaneDelta::PlaneDelta(const sky::PlaneTuning &tuning,
                            const sky::PlaneState &state) :
    tuning(tuning), state(state) { }

bool PlaneDelta::verifyStructure() const {
  return true;
//  return not (!bool(tuning) or bool(state));
}

/**
 * PlaneVital.
 */

PlaneVital::PlaneVital(Sky &parent,
                       const PlaneTuning &tuning,
                       const PlaneState &state) :
    parent(parent), physics(parent.physics),
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

void PlaneVital::tick(const float delta) {
  // helpful synonyms
  const float
      forwardVel = state.forwardVelocity(),
      velocity = state.velocity();

  // set rotation
  state.physical.rotvel = ((state.stalled) ?
                           tuning.stall.maxRotVel :
                           tuning.flight.maxRotVel)
      * movementValue(state.rotCtrl);

  state.energy += tuning.energy.recharge * delta;
  state.afterburner = 0;

  if (state.stalled) {
    // afterburner
    if (state.throtCtrl == Movement::Up) {
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
    state.throttle += movementValue(state.throtCtrl) * delta;
    bool afterburning =
        (state.throtCtrl == Movement::Up) && state.throttle == 1;

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

/**
 * Plane.
 */

void Plane::syncControls() {
  if (vital) {
    vital->state.rotCtrl = addMovement(leftState, rightState);
    vital->state.throtCtrl = addMovement(revState, thrustState);
  }
}

void Plane::beforePhysics() {
  if (vital) vital->writeToBody();
  for (auto &prop : props) prop.writeToBody();
}

void Plane::afterPhysics(float delta) {
  if (isSpawned()) {
    vital->readFromBody();
    vital->tick(delta);

    primaryCooldown.cool(delta);

    if (primaryState) {
      if (primaryCooldown && primaryState) {
        if (vital->state.requestDiscreteEnergy(
            vital->tuning.energy.laserGun)) {
          props.emplace_front(
              parent,
              vital->state.physical.pos + 100.0f * VecMath::fromAngle(
                  vital->state.physical.rot),
              vital->state.physical.vel + 300.0f * VecMath::fromAngle
                  (vital->state.physical.rot));
          primaryCooldown.reset();
        }
      }
    }
  }

  for (auto &prop : props) {
    prop.readFromBody();
    prop.tick(delta);
  }

  props.remove_if([](Prop &prop) {
    return prop.lifeTime > 1;
  });
}

void Plane::spawn(const PlaneTuning &tuning,
                  const sf::Vector2f pos,
                  const float rot) {
  vital.emplace(parent, tuning, PlaneState(tuning, pos, rot));
  syncControls();
  newlyAlive = true;
}

void Plane::doAction(const Action &action, const bool state) {
  switch (action) {
    case Action::Thrust: {
      thrustState = state;
      break;
    }
    case Action::Reverse: {
      revState = state;
      break;
    }
    case Action::Left: {
      leftState = state;
      break;
    }
    case Action::Right: {
      rightState = state;
      break;
    }
    case Action::Primary: {
      primaryState = state;
      break;
    }
    case Action::Secondary: {
      break;
    }
    case Action::Special: {
      break;
    }
    case Action::Suicide: {
      vital.reset();
      parent.caller.onDie(player);
      newlyAlive = false;
    }
  }

  syncControls();
}

void Plane::applyDelta(const PlaneDelta &delta) {
  if (delta.tuning) {
    vital.emplace(parent, delta.tuning.get(), delta.state.get());
    return;
  }
  if (delta.state) {
    if (vital) vital->state = *delta.state;
  } else vital.reset();
}

PlaneInitializer Plane::captureInitializer() const {
  if (vital) {
    return PlaneInitializer(vital->tuning, vital->state);
  } else {
    return {};
  }
}

PlaneDelta Plane::captureDelta() {
  if (newlyAlive) {
    newlyAlive = false;
    return PlaneDelta(vital->tuning, vital->state);
  } else {
    if (vital) return PlaneDelta(vital->state);
    else return PlaneDelta();
  }
}

Plane::Plane(Sky &parent, Player &player) :
    parent(parent),
    player(player),
    newlyAlive(false),
    leftState(false), rightState(false),
    thrustState(false), revState(false),
    primaryState(false),
    primaryCooldown(0.2) { }

Plane::Plane(Sky &parent, Player &player,
             const PlaneInitializer &initializer) :
    Plane(parent, player) {
  if (initializer.spawn)
    vital.emplace(parent, initializer.spawn->first, initializer.spawn->second);
}

bool Plane::isSpawned() const {
  return bool(vital);
}


}

