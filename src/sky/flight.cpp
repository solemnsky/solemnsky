#include <cmath>

#include "flight.h"
#include "sky.h"
#include "util/methods.h"

namespace sky {

/**
 * PlaneTuning.
 */

PlaneTuning::PlaneTuning() { }

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
    tuning(tuning), state(state) { }

/**
 * Plane.
 */

Plane::Plane(Sky *parent,
             const PlaneTuning &tuning,
             const sf::Vector2f pos,
             const float rot) :
    parent(parent),
    physics(&parent->physics),
    body(physics->rectBody(tuning.hitbox)),
    state(tuning, pos, rot) {
  CTOR_LOG("Plane");

  // initialize body, in agreement with state
  body = physics->rectBody(tuning.hitbox);
  body->SetAngularVelocity(toRad(state.rot));
  body->SetLinearVelocity(physics->toPhysVec(state.vel));
  body->SetGravityScale(state.stalled ? 1 : 0);
}

Plane::Plane(Sky *parent, const PlaneInitializer &initializer) :
    parent(parent),
    physics(&parent->physics),
    body(physics->rectBody(initializer.tuning.hitbox)),
    state(initializer.state) {
  CTOR_LOG("Plane");
}

Plane::Plane(Plane &&plane) :
    parent(plane.parent),
    physics(plane.physics),
    body(plane.body),
    state(plane.state) {
  plane.body = nullptr;
  CTOR_LOG("Plane");
}

Plane::~Plane() {
  physics->clrBody(body);
  DTOR_LOG("Plane");
};

void Plane::writeToBody() {
  // TODO: use proper motors / motor joints for this instead of impulses
  physics->approachRotVel(body, state.rotvel);
  physics->approachVel(body, state.vel);
  body->SetGravityScale(state.stalled ? 1 : 0);

  body->SetTransform(
      physics->toPhysVec(state.pos),
      toRad(state.rot));
}

void Plane::readFromBody() {
  state.pos = physics->toGameVec(body->GetPosition());
  state.rot = toDeg(body->GetAngle());
  state.rotvel = toDeg(body->GetAngularVelocity());
  state.vel = physics->toGameVec(body->GetLinearVelocity());
}

void Plane::tick(float delta) {
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

PlaneInitializer Plane::captureInitializer() const {
  return PlaneInitializer(tuning, state);
}

}

