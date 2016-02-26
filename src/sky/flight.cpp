#include <cmath>

#include "flight.h"
#include "sky.h"
#include "util/methods.h"

namespace sky {

/**
 * PlaneTuning.
 */

PlaneTuning::PlaneTuning() { }

#define member(TYPE, PTR, RULE) \
  tg::MemberRule<PlaneTuning::Energy, TYPE>(RULE, &PlaneTuning::Energy::PTR)
const static tg::Pack<PlaneTuning::Energy> planeTuningEnergyPack =
    tg::ClassPack<PlaneTuning::Energy>(
        member(float, thrustDrain, tg::floatPack),
        member(float, recharge, tg::floatPack),
        member(float, laserGun, tg::floatPack)
    );
#undef member

#define member(TYPE, PTR, RULE) \
  tg::MemberRule<PlaneTuning::Stall, TYPE>(RULE, &PlaneTuning::Stall::PTR)
const static tg::Pack<PlaneTuning::Stall> planeTuningStallPack =
    tg::ClassPack<PlaneTuning::Stall>(
        member(float, maxRotVel, tg::floatPack),
        member(float, maxVel, tg::floatPack),
        member(float, thrust, tg::floatPack),
        member(float, damping, tg::floatPack),
        member(float, threshold, tg::floatPack)
    );
#undef member

#define member(TYPE, PTR, RULE) \
  tg::MemberRule<PlaneTuning::Flight, TYPE>(RULE, &PlaneTuning::Flight::PTR)
const static tg::Pack<PlaneTuning::Flight> planeTuningFlightPack =
    tg::ClassPack<PlaneTuning::Flight>(
        member(float, maxRotVel, tg::floatPack),
        member(float, airspeedFactor, tg::floatPack),
        member(float, throttleInfluence, tg::floatPack),
        member(float, throttleEffect, tg::floatPack),
        member(float, gravityEffect, tg::floatPack),
        member(float, gravityEffect, tg::floatPack),
        member(float, afterburnDrive, tg::floatPack),
        member(float, leftoverDamping, tg::floatPack),
        member(float, threshold, tg::floatPack)
    );
#undef member

#define member(TYPE, PTR, RULE) \
  tg::MemberRule<PlaneTuning, TYPE>(RULE, &PlaneTuning::PTR)
PlaneTuningPack::PlaneTuningPack() :
    tg::ClassPack<PlaneTuning>(
        member(sf::Vector2f, hitbox, tg::vectorPack),
        member(PlaneTuning::Energy, energy, planeTuningEnergyPack),
        member(PlaneTuning::Stall, stall, planeTuningStallPack),
        member(PlaneTuning::Flight, flight, planeTuningFlightPack),
        member(float, throttleSpeed, tg::floatPack)
        // this is what we call "big data" around here
    ) { }
#undef member

/**
 * PlaneState.
 */

PlaneState::PlaneState() :
    rotCtrl(-1, 1),
    afterburner(0, 1),
    airspeed(0, 1),
    throttle(0, 1),
    energy(0, 1),
    health(0, 1) { }

PlaneState::PlaneState(const PlaneTuning &tuning,
                       const sf::Vector2f &pos,
                       const float rot) :
    rotCtrl(-1, 1, 0),
    throtCtrl(Movement::None),

    pos(pos),
    vel(tuning.flight.airspeedFactor * VecMath::fromAngle(rot)),
    rot(rot),
    rotvel(0),

    stalled(false), afterburner(0, 1, 0),
    airspeed(0, 1, tuning.flight.throttleInfluence),
    leftoverVel(0, 0),
    throttle(0, 1, 1),

    energy(0, 1, 1),
    health(0, 1, 1) { }

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

#define member(TYPE, PTR, RULE) \
  tg::MemberRule<PlaneState, TYPE>(RULE, &PlaneState::PTR)
PlaneStatePack::PlaneStatePack() :
    tg::ClassPack<PlaneState>(
        member(Clamped, rotCtrl, tg::clampedPack),
        member(Movement, throtCtrl, tg::movementPack),
        member(sf::Vector2f, pos, tg::vectorPack),
        member(sf::Vector2f, vel, tg::vectorPack),
        member(Angle, rot, tg::anglePack),
        member(float, rotvel, tg::floatPack),
        member(bool, stalled, tg::boolPack),
        member(Clamped, afterburner, tg::clampedPack),
        member(sf::Vector2f, leftoverVel, tg::vectorPack),
        member(Clamped, airspeed, tg::clampedPack),
        member(Clamped, throttle, tg::clampedPack),
        member(Clamped, energy, tg::clampedPack),
        member(Clamped, health, tg::clampedPack)
    ) { }
#undef member

/**
 * PlaneInitializer.
 */

PlaneInitializer::PlaneInitializer() { }

PlaneInitializer::PlaneInitializer(
    const PlaneTuning &tuning, const PlaneState &state) :
    tuning(tuning), state(state) { }

#define member(TYPE, PTR, RULE) \
  tg::MemberRule<PlaneInitializer, TYPE>(RULE, &PlaneInitializer::PTR)
PlaneInitializerPack::PlaneInitializerPack() :
    tg::ClassPack<PlaneInitializer>(
        member(PlaneTuning, tuning, PlaneTuningPack()),
        member(PlaneState, state, PlaneStatePack())
    ) { }
#undef member

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
                  tuning.flight.maxRotVel) * state.rotCtrl;

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

