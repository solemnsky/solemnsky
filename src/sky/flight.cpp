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

void Plane::spawn(const sf::Vector2f pos, const float rot) {
  state = PlaneState(physics, tuning, pos, rot);
}

void Plane::kill() {
  state = {};
}

//Plane::Plane() : engine(nullptr), physics(nullptr) {
//  // this shouldn't exist (see to-do item in header)
//}

void Plane::tick(float d) {
  if (state) {
    // the following comment is direct transcript from the haxe repository
//        // synonyms
//        var forwardVel:Float =
//            state.vel.length() * Math.cos(state.rot - state.vel.angle());
//        var speed = state.vel.length();
//
//        // rotation
//        var maxRotation =
//            if (state.stalled) mod.maxRotationStalled
//                else mod.maxRotation;
//        var targetRotVel:Float = 0;
//        if (state.movement.left) targetRotVel = -maxRotation;
//        if (state.movement.right) targetRotVel += maxRotation;
//
//        state.rotvel = targetRotVel;
//            // (targetRotVel - state.rotvel)
//            //     / Math.pow(mod.angularDamping, delta);
//
//        state.afterburner = false;
//
//        // motion when stalled
//        if (state.stalled) {
//            // add basic thrust
//            if (state.movement.forward) {
//                state.afterburner = true;
//                state.vel = Vector.fromAngle(state.rot)
//                    .mult(
//                        delta / 1000 * mod.afterburnerStalled
//                    )
//                    .add(state.vel);
//            }
//
//            // apply damping when over maxVelocityStalled
//            var excessVel = speed - mod.maxVelocityStalled;
//            var dampingFactor =
//                mod.maxVelocityStalled / speed;
//            if (excessVel > 0)
//                state.vel.y =
//                   state.vel.y * dampingFactor
//                        * Math.pow(
//                            mod.stallDamping
//                            , delta / 1000);
//
//        // motion when not stalled
//        } else {
//            // modify throttle and afterburner according to controls
//            if (state.movement.forward && state.throttle < 1)
//                state.throttle += mod.throttleSpeed *
//                    (delta / 1000);
//            if (state.movement.backward && state.throttle > 0)
//                state.throttle -= mod.throttleSpeed *
//                    (delta / 1000);
//            state.throttle = Math.min(state.throttle, 1);
//            state.throttle = Math.max(state.throttle, 0);
//            state.afterburner =
//                state.movement.forward && state.throttle == 1;
//
//            // pick away at leftover velocity
//            state.leftoverVel.x = state.leftoverVel.x
//                * Math.pow(mod.leftoverVelDamping, delta / 1000);
//            state.leftoverVel.y = state.leftoverVel.y
//                * Math.pow(mod.leftoverVelDamping, delta / 1000);
//
//            // speed modifiers
//            if (state.speed > state.throttle * mod.speedThrottleInfluence) {
//                if (state.throttle < mod.speedThrottleInfluence) {
//                    state.speed -=
//                        mod.speedThrottleDeaccForce * (delta / 1000);
//                } else {
//                    state.speed -=
//                        mod.speedThrottleForce * (delta / 1000);
//                }
//            }
//            state.speed +=
//                Math.sin(state.rot)
//                    * mod.speedGravityForce * (delta / 1000);
//            if (state.afterburner)
//                state.speed += mod.speedAfterburnForce * (delta / 1000);
//            state.speed = Math.min(state.speed, 1);
//            state.speed = Math.max(state.speed, 0);
//
//            var targetSpeed = state.speed * mod.speed;
//
//            // set velocity, according to target speed, rotation,
//            // and leftoverVel
//            state.vel = Vector.fromAngle(state.rot)
//                .mult(targetSpeed)
//                .add(state.leftoverVel);
//        }
//
//        // stall singularities
//        if (state.stalled) {
//            if (forwardVel > mod.exitStallThreshold) {
//                state.stalled = false;
//                engine.applyZeroGravity(body);
//                state.leftoverVel = new Vector(
//                    state.vel.x - forwardVel * Math.cos(state.rot)
//                    , state.vel.y - forwardVel * Math.sin(state.rot)
//                );
//                state.speed =
//                    forwardVel / mod.speed;
//                state.throttle =
//                    state.speed / mod.speedThrottleInfluence;
//            }
//        } else {
//            if (forwardVel < mod.enterStallThreshold) {
//                engine.applyGravity(body);
//                state.stalled = true;
//                state.throttle = 1;
//                state.speed = 0;
//            }
//        }
//
//        mod.onTick(delta);
  }
}
}
