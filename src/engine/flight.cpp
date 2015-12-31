#include <cmath>
#include <base/physics.h>
#include "flight.h"
#include "engine.h"

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

Plane::Plane(Engine *engine, const PlaneTuning tuning) :
    engine(engine), physics(&engine->physics), tuning(tuning) { }

void Plane::spawn(const sf::Vector2f pos, const float rot) {
  state = PlaneState(physics, tuning, pos, rot);
}

void Plane::kill() {
  state = {};
}

Plane::Plane() : engine(nullptr), physics(nullptr) {
  // this shouldn't exist (see to-do item in header)
}

void Plane::tick(float d) {
  if (state) {
    // TODO: port physics from Haxe repo
  }
}
}
