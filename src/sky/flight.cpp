#include <cmath>

#include "flight.h"
#include "sky.h"

namespace sky {

/****
 * PlaneState
 */

PlaneState::PlaneState(const PlaneTuning &tuning,
                       const sf::Vector2f &pos,
                       const float rot) :
    tuning(tuning),

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

float PlaneState::forwardVelocity() {
  return velocity() * (const float) cos(toRad(rot) - std::atan2(vel.y, vel.x));
}

float PlaneState::velocity() {
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

/****
 * Plane
 */

void Plane::writeToBody() {
  if (state) {
    if (!body) {
      body = physics->rectBody(state->tuning.hitbox);

      // hard-set these values when a body is new
      body->SetAngularVelocity(toRad(state->rot));
      body->SetLinearVelocity(physics->toPhysVec(state->vel));
      body->SetGravityScale(state->stalled ? 1 : 0);
    } else {
      // otherwise, use impulses so we're participating in physics minimally
      // (ideally we should be using motors for this, let's put that as an
      // optional TODO...)
      physics->approachRotVel(body, state->rotvel);
      physics->approachVel(body, state->vel);
      body->SetGravityScale(state->stalled ? 1 : 0);
    }
    body->SetTransform(
        physics->toPhysVec(state->pos),
        toRad(state->rot));
  } else {
    if (body) {
      physics->clrBody(body);
    }
  }
}

void Plane::readFromBody() {
  if (!state) return;

  if (body) {
    state->pos = physics->toGameVec(body->GetPosition());
    state->rot = toDeg(body->GetAngle());
    state->rotvel = toDeg(body->GetAngularVelocity());
    state->vel = physics->toGameVec(body->GetLinearVelocity());
  }
}

void Plane::tick(float delta) {
  if (!state) return;

  // helpful synonyms
  const auto &tuning = state->tuning;
  const float forwardVel(state->forwardVelocity()), velocity(state->velocity());

  // set rotation
  state->rotvel = ((state->stalled) ?
                   tuning.stall.maxRotVel :
                   tuning.flight.maxRotVel) * state->rotCtrl;

  state->energy += tuning.energy.recharge * delta;
  state->afterburner = 0;

  if (state->stalled) {
    // afterburner
    if (state->throtCtrl == 1) {
      const float thrustEfficacity =
          state->requestEnergy(tuning.energy.thrustDrain * delta);

      state->vel +=
          VecMath::fromAngle(state->rot) *
          (delta * tuning.stall.thrust * thrustEfficacity);
      state->afterburner = thrustEfficacity;
    }

    // damping towards terminal velocity
    float excessVel = velocity - tuning.stall.maxVel;
    float dampingFactor = tuning.stall.maxVel / velocity;
    if (excessVel > 0)
      state->vel = state->vel * dampingFactor *
                   std::pow(tuning.stall.damping, delta);
  } else {
    // modify throttle and afterburner according to controls
    state->throttle += state->throtCtrl * delta;
    bool afterburning = (state->throtCtrl == 1) && state->throttle == 1;

    // pick away at leftover velocity
    state->leftoverVel *= std::pow(tuning.flight.leftoverDamping, delta);

    // modify state->airspeed
    float speedMod = 0;
    speedMod += sin(toRad(state->rot)) * tuning.flight.gravityEffect * delta;
    if (afterburning) {
      const float thrustEfficacity =
          state->requestEnergy(tuning.energy.thrustDrain * delta);
      state->afterburner = thrustEfficacity;
      speedMod += tuning.flight.afterburnDrive * delta * thrustEfficacity;
    }
    state->airspeed += speedMod;

    approach(state->airspeed,
             (float) state->throttle * tuning.flight.throttleInfluence,
             tuning.flight.throttleDrive * delta);

    float targetSpeed = state->airspeed * tuning.flight.airspeedFactor;

    // set velocity, according to target speed, rotation, and leftoverVel
    state->vel = targetSpeed * VecMath::fromAngle(state->rot) +
                 state->leftoverVel;
  }

  // stall singularities
  if (state->stalled) {
    if (forwardVel > tuning.stall.threshold) {
      state->stalled = false;
      state->leftoverVel =
          state->vel - (forwardVel * VecMath::fromAngle(state->rot));

      state->airspeed = forwardVel / tuning.flight.airspeedFactor;
      state->throttle = state->airspeed / tuning.flight.throttleInfluence;
    }
  } else {
    if (forwardVel < tuning.flight.threshold) {
      state->stalled = true;
      state->throttle = 1;
      state->airspeed = 0;
    }
  }
}

Plane::Plane(Sky *engine) :
    engine(engine), physics(&engine->physics), body(nullptr) {
  CTOR_LOG("plane");
}

Plane::~Plane() {
  kill();
  DTOR_LOG("plane");
};


void Plane::spawn(const sf::Vector2f pos, const float rot,
                  const PlaneTuning &tuning) {
  if (state) kill();
  state = {PlaneState(tuning, pos, rot)};
  writeToBody();
}

void Plane::kill() {
  state = boost::optional<PlaneState>();
  writeToBody();
}


}
