#include <cmath>

#include "flight.h"
#include "sky.h"
#include "util/methods.h"

namespace sky {

/****
 * PlaneVital
 */

PlaneVital::PlaneVital(const PlaneTuning &tuning,
                       const sf::Vector2f &pos,
                       const float rot) :
    rotCtrl(-1, 1, 0),
    throtCtrl({-1, 0, 1}, 0),

    pos(pos),
    vel(tuning.flight.airspeedFactor * VecMath::fromAngle(rot)),
    rot(rot),
    rotvel(0),

    stalled(false), afterburner(0, 1, 0),
    airspeed(0, 1, tuning.flight.throttleInfluence),
    leftoverVel(0, 0),
    throttle(0, 1, 1),

    energy(0, 1, 1),
    health(0, tuning.maxHealth, tuning.maxHealth) { }

float PlaneVital::forwardVelocity() {
  return velocity() * (const float) cos(toRad(rot) - std::atan2(vel.y, vel.x));
}

float PlaneVital::velocity() {
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

/****
 * Plane
 */

/****
 * PlaneHandle
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
      // (ideally we should be using motors for this, let's put that as an
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
  auto &vstate = state.vital; // woo saved one character

  if (!vstate) return;

  // helpful synonyms
  const auto &tuning = state.tuning;
  const float forwardVel(vstate->forwardVelocity()), velocity(
      vstate->velocity());

  // set rotation
  vstate->rotvel = ((vstate->stalled) ?
                    tuning.stall.maxRotVel :
                    tuning.flight.maxRotVel) * vstate->rotCtrl;

  vstate->energy += tuning.energy.recharge * delta;
  vstate->afterburner = 0;

  if (vstate->stalled) {
    // afterburner
    if (vstate->throtCtrl == 1) {
      const float thrustEfficacy =
          vstate->requestEnergy(tuning.energy.thrustDrain * delta);

      vstate->vel +=
          VecMath::fromAngle(vstate->rot) *
          (delta * tuning.stall.thrust * thrustEfficacy);
      vstate->afterburner = thrustEfficacy;
    }

    // damping towards terminal velocity
    float excessVel = velocity - tuning.stall.maxVel;
    float dampingFactor = tuning.stall.maxVel / velocity;
    if (excessVel > 0)
      vstate->vel = vstate->vel * dampingFactor *
                    std::pow(tuning.stall.damping, delta);
  } else {
    // modify throttle and afterburner according to controls
    vstate->throttle += vstate->throtCtrl * delta;
    bool afterburning =
        (vstate->throtCtrl == 1) && vstate->throttle == 1;

    // pick away at leftover velocity
    vstate->leftoverVel *= std::pow(tuning.flight.leftoverDamping, delta);

    // modify vstate->airspeed
    float speedMod = 0;
    speedMod +=
        sin(toRad(vstate->rot)) * tuning.flight.gravityEffect * delta;
    if (afterburning) {
      const float thrustEfficacity =
          vstate->requestEnergy(tuning.energy.thrustDrain * delta);
      vstate->afterburner = thrustEfficacity;
      speedMod += tuning.flight.afterburnDrive * delta * thrustEfficacity;
    }
    vstate->airspeed += speedMod;

    approach(vstate->airspeed,
             (float) vstate->throttle * tuning.flight.throttleInfluence,
             tuning.flight.throttleDrive * delta);

    float targetSpeed = vstate->airspeed * tuning.flight.airspeedFactor;

    // set velocity, according to target speed, rotation, and leftoverVel
    vstate->vel = targetSpeed * VecMath::fromAngle(vstate->rot) +
                  vstate->leftoverVel;
  }

  // stall singularities
  if (vstate->stalled) {
    if (forwardVel > tuning.stall.threshold) {
      vstate->stalled = false;
      vstate->leftoverVel =
          vstate->vel - (forwardVel * VecMath::fromAngle(vstate->rot));

      vstate->airspeed = forwardVel / tuning.flight.airspeedFactor;
      vstate->throttle =
          vstate->airspeed / tuning.flight.throttleInfluence;
    }
  } else {
    if (forwardVel < tuning.flight.threshold) {
      vstate->stalled = true;
      vstate->throttle = 1;
      vstate->airspeed = 0;
    }
  }
}

PlaneHandle::PlaneHandle(Sky *engine) :
    engine(engine), physics(&engine->physics), body(nullptr) {
  CTOR_LOG("plane");
}

PlaneHandle::~PlaneHandle() {
  kill();
  DTOR_LOG("plane");
};

void PlaneHandle::spawn(const PlaneTuning &tuning,
                        const sf::Vector2f pos,
                        const float rot) {
  if (state.vital) kill();
  state.vital.emplace(PlaneVital(tuning, pos, rot));
  state.tuning = tuning;
  writeToBody();
}

void PlaneHandle::kill() {
  state.vital.reset();
  writeToBody();
}

}
