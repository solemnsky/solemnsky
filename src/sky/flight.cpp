#include <cmath>
#include "flight.h"
#include "sky.h"

namespace sky {

/****
 * PlaneState
 */

PlaneState::PlaneState(Physics *physics, const PlaneTuning tuning,
                       const sf::Vector2f pos, const float rot)
    : pos(pos),
      rot(rot) {
  vel = tuning.flight.maxVelCruise * sf::Vector2f(std::cos(rot), std::sin(rot));
  throttle = tuning.throttleSize;
  speed = tuning.flight.maxVelCruise;
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
    if (!body) body = physics->planeBody(physics->toPhysVec(tuning.hitbox));

    body->SetTransform(
        physics->toPhysVec(state->pos),
        state->rot);
    body->SetAngularVelocity(state->rotvel);
    body->SetLinearVelocity(physics->toPhysVec(state->vel));
  } else {
    if (body) physics->world.DestroyBody(body);
  }
}

void Plane::readFromBody() {
}

Plane::Plane(Sky *engine, const PlaneTuning tuning) :
    engine(engine), physics(&engine->physics), tuning(tuning) { }

Plane::~Plane() { physics->world.DestroyBody(body); };

void Plane::spawn(const sf::Vector2f pos, const float rot) {
  state = PlaneState(physics, tuning, pos, rot);
}

void Plane::kill() {
  state = {};
}

void Plane::tick(float delta) {
  if (state) {
    // the following comment is direct transcript from the haxe repository

    const float forwardVel(state->forwardVelocity()), speed(state->velocity());

    const float targetRotVel =
        ((state->stalled) ? tuning.stall.maxRot : tuning.flight.maxRot) *
        state->rotCtrl;
    state->rotvel = targetRotVel; // TODO: damping?

    state->afterburner = false; // true afterburner value is set explicitly

    if (state->stalled) {
      // add basic thrust
      if (state->throtCtrl != 0) {
        state->afterburner = true;
        state->vel +=
            VecMath::fromAngle(state->rot) * (delta * tuning.stall.maxThrust);
      }

      // apply damping when over maxVelocityStalled
      float excessVel = speed - tuning.stall.maxVel;
      float dampingFactor = tuning.stall.maxVel / speed;
      if (excessVel > 0)
        state->vel.y =
            state->vel.y * dampingFactor *
            std::pow(tuning.stall.damping, delta);


    } else { // motion when not stalled

      // modify throttle and afterburner according to controls
      if (state->throtCtrl == 1) state->throttle += delta;
      if (state->throtCtrl == -1) state->throttle -= delta;
      state->throttle = clamp(0f, tuning.throttleSize, 0);

      state->afterburner = (state->throtCtrl == 1) && state->throttle == 1;

      // pick away at leftover velocity
      state->leftoverVel *= std::pow(tuning.flight.leftoverVelDamping, delta);

      // speed modifiers
      if (state->speed >
          state->throttle * tuning.velInfluence.throttleInfluence) {
        if (state->throttle < tuning.velInfluence.throttleInfluence) {
          state->speed -= tuning.velInfluence.throttleDown * delta;
        } else {
          state->speed -= tuning.velInfluence.throttleUp * delta;
        }
      }

      state.speed +=
          sin(state.rot) * tuning.velInfluence.speedGravityForce * (delta /
                                                                    1000);
      if (state.afterburner)
        state.speed += mod.speedAfterburnForce * (delta / 1000);
      state.speed = Math.min(state.speed, 1);
      state.speed = Math.max(state.speed, 0);

      var targetSpeed = state.speed * mod.speed;

      // set velocity, according to target speed, rotation,
      // and leftoverVel
      state.vel = Vector.fromAngle(state.rot)
          .mult(targetSpeed)
          .add(state.leftoverVel);
    }

    // stall singularities
    if (state.stalled) {
      if (forwardVel > mod.exitStallThreshold) {
        state.stalled = false;
        engine.applyZeroGravity(body);
        state.leftoverVel = new Vector(
            state.vel.x - forwardVel * Math.cos(state.rot),
            state.vel.y - forwardVel * Math.sin(state.rot)
        );
        state.speed =
            forwardVel / mod.speed;
        state.throttle =
            state.speed / mod.speedThrottleInfluence;
      }
    } else {
      if (forwardVel < mod.enterStallThreshold) {
        engine.applyGravity(body);
        state.stalled = true;
        state.throttle = 1;
        state.speed = 0;
      }
    }
//
//        mod.onTick(delta);
  }
}

}
