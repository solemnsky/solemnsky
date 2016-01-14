#include <cmath>

#include "flight.h"
#include "sky.h"

namespace sky {

/****
 * PlaneState
 */

PlaneState::PlaneState(Physics *physics, const PlaneTuning &tuning,
                       const sf::Vector2f &pos, const float rot) :
    rotCtrl(-1, 1, 0),
    throtCtrl({-1, 0, 1}, 0),
    pos(pos),
    rot(rot),
    tuning(tuning),
    throttle(0, 1, 1),
    vel(tuning.flight.speed * VecMath::fromAngle(rot)),
    rotvel(0),
    stalled(false),
    afterburner(false),
    leftoverVel(0, 0),
    speed(0, 1, tuning.flight.speedThrottleInfluence) { }

float PlaneState::forwardVelocity() {
  return (float) (velocity() * sin(toRad(rot)));
}

float PlaneState::velocity() {
  return VecMath::length(vel);
}

/****
 * PlaneAnimState
 */

PlaneAnimState::PlaneAnimState() :
    roll(0),
    orientation(false),
    flipState(0),
    rollState(0) { }

void PlaneAnimState::tick(Plane *parent, const float delta) {
  auto &state = parent->state;
  if (state) {
    bool newOrientation = Angle(state->rot + 90) > 180;
    if (state->rotCtrl == 0
        and newOrientation != orientation
        and state->stalled) {
      orientation = newOrientation;
    }

    approach(flipState,
             (const float) (orientation ? 1 : 0),
             2 * delta);

    Angle flipComponent;
    if (orientation) flipComponent = 90 - flipState * 180;
    else flipComponent = 90 + flipState * 180;

    bool rolling(false);
    if (flipState == 0 || flipState == 1) {
      if (state->rotCtrl == -1) {
        approach(rollState, -1.0f, 2 * delta);
        rolling = true;
      } else if (state->rotCtrl == 1) {
        approach(rollState, 1.0f, 2 * delta);
        rolling = true;
      }
    }
    if (!rolling) approach(rollState, 0.0f, 2 * delta);

    const float rollComponent = 30 * rollState;

    roll = flipComponent + rollComponent;
  }
}

void PlaneAnimState::reset() {
  operator=((PlaneAnimState &&) PlaneAnimState()); // using the move operator
  // for no reason at all except to make everybody know that I've been
  // reading about the C++11 spec
}

/****
 * Plane
 */

void Plane::writeToBody() {
  if (state) {
    if (!body) {
      body = physics->rectBody(state->tuning.hitbox);
      body->SetTransform(
          physics->toPhysVec(state->pos),
          toRad(state->rot));
      body->SetAngularVelocity(toRad(state->rot));
      body->SetLinearVelocity(physics->toPhysVec(state->vel));
      body->SetGravityScale(state->stalled ? 1 : 0);
    } else {
      body->SetTransform(
          physics->toPhysVec(state->pos),
          toRad(state->rot));
      physics->approachRotVel(body, state->rotvel);
      physics->approachVel(body, state->vel);
      body->SetGravityScale(state->stalled ? 1 : 0);
    }
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
  if (state) {
    const PlaneTuning &tuning = state->tuning;
    const float forwardVel(state->forwardVelocity()),
        velocity(state->velocity());

    // set rotation
    state->rotvel = ((state->stalled) ?
                     tuning.stall.maxRotVel :
                     tuning.flight.maxRotVel) * state->rotCtrl;

    state->afterburner = false;
    if (state->stalled) {

      // afterburner
      if (state->throtCtrl == 1) {
        state->vel +=
            VecMath::fromAngle(state->rot) * (delta * tuning.stall.thrust);
        state->afterburner = true;
      }

      // damping towards terminal velocity
      float excessVel = velocity - tuning.stall.maxVel;
      float dampingFactor = tuning.stall.maxVel / velocity;
      if (excessVel > 0)
        state->vel = state->vel * dampingFactor *
                     std::pow(tuning.stall.damping, delta);

    } else {

      // modify throttle and afterburner according to controls
      if (state->throtCtrl == 1) state->throttle += delta;
      if (state->throtCtrl == -1) state->throttle -= delta;
      state->afterburner = (state->throtCtrl == 1) && state->throttle == 1;

      // pick away at leftover velocity
      state->leftoverVel *= std::pow(tuning.flight.damping, delta);

      // speed modifiers
      float speedMod = 0;
      if (state->speed >
          state->throttle * tuning.flight.speedThrottleInfluence) {
        if (state->throttle < tuning.flight.speedThrottleInfluence)
          speedMod -= tuning.flight.speedThrottleDeaccForce * delta;
        else speedMod += tuning.flight.speedThrottleForce * delta;
      }
      speedMod += sin(state->rot) * tuning.flight.speedGravityForce * delta;
      speedMod += ((float) state->afterburner) * // definitely readable
                      tuning.flight.speedAfterburnForce * delta;
      state->speed += speedMod;
      float targetSpeed = state->speed * tuning.flight.speed;

      // set velocity, according to target speed, rotation, and leftoverVel
      state->vel = targetSpeed * VecMath::fromAngle(state->rot) +
                   state->leftoverVel;
    }

    // stall singularities
    if (state->stalled) {
      if (forwardVel > tuning.stall.threshold) {
        state->stalled = false;
        state->leftoverVel = state->vel - (forwardVel * VecMath::fromAngle
            (state->rot));

        state->speed = forwardVel / tuning.flight.speed;
        state->throttle = state->speed / tuning.flight.speedThrottleInfluence;
      }
    } else {
      if (forwardVel < tuning.flight.threshold) {
        state->stalled = true;
        state->throttle = 1;
        state->speed = 0;
      }
    }
  }
}

void Plane::tickAnim(float delta) {
  animState.tick(this, delta);
}

Plane::Plane(Sky *engine) :
    engine(engine), physics(&engine->physics), body(nullptr) {
  LIFE_LOG("Creating plane.");
}

Plane::~Plane() {
  kill();
  LIFE_LOG("Destroying plane.");
};


void Plane::spawn(const sf::Vector2f pos, const float rot,
                  const PlaneTuning &tuning) {
  if (state) kill();
  animState.reset();
  state = {PlaneState(physics, tuning, pos, rot)};
  writeToBody();
}

void Plane::kill() {
  state = {};
  writeToBody();
}


}
