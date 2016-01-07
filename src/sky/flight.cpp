#include <cmath>

#include "flight.h"
#include "sky.h"

namespace sky {

/****
 * PlaneState
 */

PlaneState::PlaneState(Physics *physics, const PlaneTuning &tuning,
                       const sf::Vector2f &pos, const float rot)
    : rotCtrl(-1, 1, 0),
      throtCtrl({-1, 0, 1}, 0),
      pos(pos),
      rot(rot),
      tuning(tuning),
      throttle(0, tuning.throttleSize, tuning.throttleSize) {
  vel = tuning.flight.cruise * sf::Vector2f(std::cos(rot), std::sin(rot));
  rotvel = 0;

  stalled = false;
  afterburner = false;
  leftoverVel = {0, 0};

  speed = tuning.flight.cruise;
}

float PlaneState::forwardVelocity() {
  return (float) (velocity() * sin(rot));
}

float PlaneState::velocity() {
  return VecMath::length(vel);
}

/****
 * Plane
 */

void Plane::writeToBody() {
  if (state) {
    if (!body)
      body = physics->planeBody(state->tuning.hitbox);

    body->SetTransform(
        physics->toPhysVec(state->pos),
        state->rot);
    body->SetAngularVelocity(state->rotvel);
    body->SetLinearVelocity(physics->toPhysVec(state->vel));
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
    state->rot = body->GetAngle();
    state->rotvel = body->GetAngularVelocity();
    state->vel = physics->toGameVec(body->GetLinearVelocity());
  }
}

Plane::Plane(Sky *engine) :
    engine(engine), physics(&engine->physics) {
}

Plane::~Plane() { physics->clrBody(body); };

void Plane::spawn(const sf::Vector2f pos, const float rot,
                  const PlaneTuning &tuning) {
  state = {PlaneState(physics, tuning, pos, rot)};
  writeToBody();
}

void Plane::kill() {
  state = {};
  writeToBody();
}

void Plane::tick(float delta) {
  if (state) {
    // synonyms
    const PlaneTuning &tuning = state->tuning;
    const float forwardVel(state->forwardVelocity()),
        velocity(state->velocity());

    /**
     * Rotation.
     */
    const float targetRotVel = ((state->stalled) ?
                                tuning.stall.maxRotVel :
                                tuning.flight.maxRotVel) * state->rotCtrl;
    state->rotvel = 100; // targetRotVel;

    state->afterburner = false; // true afterburner value is set explicitly
    if (state->stalled) {
      /**
       * Afterburner.
       */
      if (state->throtCtrl == 1) {
        state->afterburner = true;
        state->vel +=
            VecMath::fromAngle(state->rot) * (delta * tuning.stall.thrust);
      }

      /**
       * Damp towards terminal velocity.
       */
      float excessVel = velocity - tuning.stall.maxVel;
      float dampingFactor = tuning.stall.maxVel / velocity;
      if (excessVel > 0)
        state->vel.y =
            state->vel.y * dampingFactor *
            std::pow(tuning.stall.damping, delta);
    }


//    } else { // motion when not stalled
//
//      // modify throttle and afterburner according to controls
//      if (state->throtCtrl == 1) state->throttle += delta;
//      if (state->throtCtrl == -1) state->throttle -= delta;
//      state->throttle = clamp(0f, tuning.throttleSize, 0);
//
//      state->afterburner = (state->throtCtrl == 1) && state->throttle == 1;
//
//      // pick away at leftover velocity
//      state->leftoverVel *= std::pow(tuning.flight.leftoverVelDamping, delta);
//
//      // speed modifiers
//      if (state->speed >
//          state->throttle * tuning.velInfluence.throttleInfluence) {
//        if (state->throttle < tuning.velInfluence.throttleInfluence) {
//          state->speed -= tuning.velInfluence.throttleDown * delta;
//        } else {
//          state->speed -= tuning.velInfluence.throttleUp * delta;
//        }
//      }
//
//      state.speed +=
//          sin(state.rot) * tuning.velInfluence.speedGravityForce * (delta /
//                                                                    1000);
//      if (state.afterburner)
//        state.speed += mod.speedAfterburnForce * (delta / 1000);
//      state.speed = Math.min(state.speed, 1);
//      state.speed = Math.max(state.speed, 0);
//
//      var targetSpeed = state.speed * mod.speed;
//
//      // set velocity, according to target speed, rotation,
//      // and leftoverVel
//      state.vel = Vector.fromAngle(state.rot)
//          .mult(targetSpeed)
//          .add(state.leftoverVel);
//    }
//
//    // stall singularities
//    if (state.stalled) {
//      if (forwardVel > mod.exitStallThreshold) {
//        state.stalled = false;
//        engine.applyZeroGravity(body);
//        state.leftoverVel = new Vector(
//            state.vel.x - forwardVel * Math.cos(state.rot),
//            state.vel.y - forwardVel * Math.sin(state.rot)
//        );
//        state.speed =
//            forwardVel / mod.speed;
//        state.throttle =
//            state.speed / mod.speedThrottleInfluence;
//      }
//    } else {
//      if (forwardVel < mod.enterStallThreshold) {
//        engine.applyGravity(body);
//        state.stalled = true;
//        state.throttle = 1;
//        state.speed = 0;
//      }
//    }
//
//        mod.onTick(delta);

  }
}

}
