#include <cmath>
#include "arena.h"
#include "plane.h"
#include "sky.h"
#include "util/methods.h"

namespace sky {

/**
 * Action.
 */

Action::Action(const Action::Type type, bool value) :
    type(type), value(value) { }


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

std::string Action::show() const {
  return actionNames.at((size_t) type) + (value ? "+" : "-");
}

optional<Action> Action::read(const std::string &str) {
  Type type = Type::Thrust;
  for (const auto &search : actionNames) {
    if ((search + "+") == str) return Action(type, true);
    if ((search + "-") == str) return Action(type, false);
    type = Type(char(type) + 1);
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

    pos(pos),
    vel(tuning.flight.airspeedFactor * VecMath::fromAngle(rot)),
    rot(rot),
    rotvel(0),

    stalled(false), afterburner(0),
    airspeed(tuning.flight.throttleInfluence),
    leftoverVel(0, 0),
    throttle(1),

    energy(1),
    health(1) { }

float PlaneState::forwardVelocity() const {
  return velocity() * (const float) cos(toRad(rot) - std::atan2(vel.y, vel.x));
}

float PlaneState::velocity() const {
  return VecMath::length(vel);
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
  return not (!bool(tuning) or bool(state));
}

/**
 * PlaneVital.
 */

PlaneVital::PlaneVital(Sky &parent,
                       const PlaneTuning &tuning,
                       const PlaneState &state) :
    parent(parent), physics(parent.physics),
    tuning(tuning), state(state) {
  body = physics.rectBody(tuning.hitbox);
  body->SetAngularVelocity(toRad(state.rot));
  body->SetLinearVelocity(physics.toPhysVec(state.vel));
  body->SetGravityScale(state.stalled ? 1 : 0);
}

PlaneVital::~PlaneVital() {
  physics.clrBody(body);
};

void PlaneVital::writeToBody() {
  // TODO: use proper motors / motor joints for this instead of impulses
  physics.approachRotVel(body, state.rotvel);
  physics.approachVel(body, state.vel);
  body->SetGravityScale(state.stalled ? 1 : 0);

  body->SetTransform(
      physics.toPhysVec(state.pos),
      toRad(state.rot));
}

void PlaneVital::readFromBody() {
  state.pos = physics.toGameVec(body->GetPosition());
  state.rot = toDeg(body->GetAngle());
  state.rotvel = toDeg(body->GetAngularVelocity());
  state.vel = physics.toGameVec(body->GetLinearVelocity());
}

void PlaneVital::tick(const float delta) {
  // helpful synonyms
  const float
      forwardVel = state.forwardVelocity(),
      velocity = state.velocity();

  // set rotation
  state.rotvel = ((state.stalled) ?
                  tuning.stall.maxRotVel :
                  tuning.flight.maxRotVel) * movementValue(state.rotCtrl);

  state.energy += tuning.energy.recharge * delta;
  state.afterburner = 0;

  if (state.stalled) {
    // afterburner
    if (state.throtCtrl == Movement::Up) {
      const float thrustEfficacy =
          state.requestEnergy(tuning.energy.thrustDrain * delta);

      state.vel +=
          VecMath::fromAngle(state.rot) *
              (delta * tuning.stall.thrust * thrustEfficacy);
      state.afterburner = thrustEfficacy;
    }

    // damping towards terminal velocity
    float excessVel = velocity - tuning.stall.maxVel;
    float dampingFactor = tuning.stall.maxVel / velocity;
    if (excessVel > 0)
      state.vel = state.vel * dampingFactor *
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
        sin(toRad(state.rot)) * tuning.flight.gravityEffect * delta;
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
    state.vel = targetSpeed * VecMath::fromAngle(state.rot) +
        state.leftoverVel;
  }

  // stall singularities
  if (state.stalled) {
    if (forwardVel > tuning.stall.threshold) {
      state.stalled = false;
      state.leftoverVel =
          state.vel - (forwardVel * VecMath::fromAngle(state.rot));

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

void Plane::syncCtrls() {
  if (vital) {
    vital->state.rotCtrl = addMovement(leftState, rightState);
    vital->state.throtCtrl = addMovement(revState, thrustState);
  }
}

void Plane::beforePhysics() {
  if (vital) vital->writeToBody();
}

void Plane::afterPhysics(float delta) {
  if (vital) {
    vital->readFromBody();
    vital->tick(delta);
  }
}

Plane::Plane(Sky &parent, Player &player) :
    parent(parent),
    player(player),
    newlyAlive(false),
    leftState(false), rightState(false),
    thrustState(false), revState(false) { }

Plane::Plane(Sky &parent, Player &player,
             const PlaneInitializer &initializer) :
    Plane(parent, player) {
  if (initializer.spawn)
    vital.emplace(parent, initializer.spawn->first, initializer.spawn->second);
}

void Plane::spawn(const PlaneTuning &tuning,
                  const sf::Vector2f pos,
                  const float rot) {
  vital.emplace(parent, tuning, PlaneState(tuning, pos, rot));
  syncCtrls();
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
      parent.arena.caller.kill(player);
      newlyAlive = false;
    }
  }

  syncCtrls();
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

}

