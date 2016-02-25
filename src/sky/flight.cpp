#include <cmath>

#include "flight.h"
#include "sky.h"
#include "util/methods.h"

namespace sky {

/**
 * PlaneTuning.
 */

#define member(TYPE, PTR, RULE) \
  tg::MemberRule<PlaneTuning::Energy, TYPE>(RULE, &PlaneTuning::Energy::PTR)
const static tg::Pack<PlaneTuning::Energy> planeTuningEnergyPack =
    tg::ClassPack<PlaneTuning::Energy>(
        member(float,
               thrustDrain,
               tg::floatPack),
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
        member(float, throttleBreaking, tg::floatPack),
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
    ) { }
#undef member

/**
 * PlaneVital
 */

PlaneVital::PlaneVital() :
    rotCtrl(-1, 1),
    afterburner(0, 1),
    airspeed(0, 1),
    throttle(0, 1),
    energy(0, 1),
    health(0, 1) { }

PlaneVital::PlaneVital(const PlaneTuning &tuning,
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

float PlaneVital::forwardVelocity() const {
  return velocity() * (const float) cos(toRad(rot) - std::atan2(vel.y, vel.x));
}

float PlaneVital::velocity() const {
  return VecMath::length(vel);
}

bool PlaneVital::requestDiscreteEnergy(const float reqEnergy) {
  if (energy < reqEnergy) return false;
  energy -= reqEnergy;
  return true;
}

float PlaneVital::requestEnergy(const float reqEnergy) {
  const float initEnergy = energy;
  energy -= reqEnergy;
  return (initEnergy - energy) / reqEnergy;
}

#define member(TYPE, PTR, RULE) \
  tg::MemberRule<PlaneVital, TYPE>(RULE, &PlaneVital::PTR)
PlaneVitalPack::PlaneVitalPack() :
    tg::ClassPack<PlaneVital>(
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
 * Plane.
 */

Plane::Plane() { }

Plane::Plane(const PlaneTuning &tuning, const PlaneVital &vital) :
    tuning(tuning), vital(vital) { }

#define member(TYPE, PTR, RULE) \
  tg::MemberRule<Plane, TYPE>(RULE, &Plane::PTR)
PlanePack::PlanePack() :
    tg::ClassPack<Plane>(
        member(PlaneTuning, tuning, PlaneTuningPack()),
        member(optional<PlaneVital>, vital, PlaneVitalPack())
    ) { }
#undef member

/****
 * PlaneHandle.
 */

void PlaneHandle::writeToBody() {
  auto &vstate = state.vital;

  if (vstate) {
    if (!body) {
      body = physics->rectBody(state.tuning.hitbox);

      // hard-set these values when a body is new
      body->SetAngularVelocity(toRad(vstate->rot));
      body->SetLinearVelocity(physics->toPhysVec(vstate->vel));
      body->SetGravityScale(vstate->stalled ? 1 : 0);
    } else {
      // otherwise, use impulses so we're participating in physics minimally
      // (ideally we should be using motor joints for this, let's put that as an
      // optional TODO...)
      physics->approachRotVel(body, vstate->rotvel);
      physics->approachVel(body, vstate->vel);
      body->SetGravityScale(vstate->stalled ? 1 : 0);
    }
    body->SetTransform(
        physics->toPhysVec(vstate->pos),
        toRad(vstate->rot));
  } else {
    if (body) {
      physics->clrBody(body);
    }
  }
}

void PlaneHandle::readFromBody() {
  auto &vstate = state.vital;

  if (!vstate) return;

  if (body) {
    vstate->pos = physics->toGameVec(body->GetPosition());
    vstate->rot = toDeg(body->GetAngle());
    vstate->rotvel = toDeg(body->GetAngularVelocity());
    vstate->vel = physics->toGameVec(body->GetLinearVelocity());
  }
}

void PlaneHandle::tick(float delta) {
  auto &vstate = state.vital;

  // helpful synonyms
  const auto &tuning = state.tuning;
  const float forwardVel(vstate.forwardVelocity()), velocity(
      vstate.velocity());

  // set rotation
  vstate.rotvel = ((vstate.stalled) ?
                   tuning.stall.maxRotVel :
                   tuning.flight.maxRotVel) * vstate.rotCtrl;

  vstate.energy += tuning.energy.recharge * delta;
  vstate.afterburner = 0;

  if (vstate.stalled) {
    // afterburner
    if (vstate.throtCtrl == Movement::Up) {
      const float thrustEfficacy =
          vstate.requestEnergy(tuning.energy.thrustDrain * delta);

      vstate.vel +=
          VecMath::fromAngle(vstate.rot) *
              (delta * tuning.stall.thrust * thrustEfficacy);
      vstate.afterburner = thrustEfficacy;
    }

    // damping towards terminal velocity
    float excessVel = velocity - tuning.stall.maxVel;
    float dampingFactor = tuning.stall.maxVel / velocity;
    if (excessVel > 0)
      vstate.vel = vstate.vel * dampingFactor *
          std::pow(tuning.stall.damping, delta);
  } else {
    // modify throttle and afterburner according to controls
    vstate.throttle += movementValue(vstate.throtCtrl) * delta;
    bool afterburning =
        (vstate.throtCtrl == Movement::Up) && vstate.throttle == 1;

    // pick away at leftover velocity
    vstate.leftoverVel *= std::pow(tuning.flight.leftoverDamping, delta);

    // modify vstate.airspeed
    float speedMod = 0;
    speedMod +=
        sin(toRad(vstate.rot)) * tuning.flight.gravityEffect * delta;
    if (afterburning) {
      const float thrustEfficacity =
          vstate.requestEnergy(tuning.energy.thrustDrain * delta);
      vstate.afterburner = thrustEfficacity;
      speedMod += tuning.flight.afterburnDrive * delta * thrustEfficacity;
    }
    vstate.airspeed += speedMod;

    approach(vstate.airspeed,
             (float) vstate.throttle * tuning.flight.throttleInfluence,
             tuning.flight.throttleEffect * delta);

    float targetSpeed = vstate.airspeed * tuning.flight.airspeedFactor;

    // set velocity, according to target speed, rotation, and leftoverVel
    vstate.vel = targetSpeed * VecMath::fromAngle(vstate.rot) +
        vstate.leftoverVel;
  }

  // stall singularities
  if (vstate.stalled) {
    if (forwardVel > tuning.stall.threshold) {
      vstate.stalled = false;
      vstate.leftoverVel =
          vstate.vel - (forwardVel * VecMath::fromAngle(vstate.rot));

      vstate.airspeed = forwardVel / tuning.flight.airspeedFactor;
      vstate.throttle =
          vstate.airspeed / tuning.flight.throttleInfluence;
    }
  } else {
    if (forwardVel < tuning.flight.threshold) {
      vstate.stalled = true;
      vstate.throttle = 1;
      vstate.airspeed = 0;
    }
  }
}

PlaneHandle::PlaneHandle(Sky *engine) :
    engine(engine), physics(&engine->physics), body(nullptr) {
  CTOR_LOG("plane");
}

PlaneHandle::PlaneHandle(Sky *engine,
                         const PlaneTuning &tuning,
                         const sf::Vector2f pos,
                         const float rot) :
    engnine(engine), physics(&engine->physics), body(nullptr),
    state() {

  CTOR_LOG("PlaneHandle");
}

PlaneHandle::~PlaneHandle() {
  physics->clrBody(body);
  DTOR_LOG("PlaneHandle");
};


PlaneHandle::PlaneHandle(PlaneHandle &&handle) :
    engine(handle.engine),
    physics(handle.physics),
    body(handle.body),
    state(handle.state) { }

}
