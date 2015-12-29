#include "flight.h"
#include "engine.h"

namespace sky {

/****
 * PlaneState
 */

PlaneState::PlaneState(Physics &physics, const PlaneTuning tuning,
                       b2Vec2 pos, float rot) : pos(pos), rot(rot) {
  vel = physics.scalar * tuning.flight.maxVelCruise *
        b2Vec2(std::cos(rot), std::sin(rot));
  throttle = tuning.throttleSize;
  speed = tuning.flight.maxVelCruise;
}

/****
 * Plane
 */

void Plane::writeToBody() {
  if (state) {
    if (!body) body = physics.planeBody(tuning.hitbox);

    body->SetTransform((1 / physics.scalar) * state->pos, state->rot);
    body->SetAngularVelocity(state->rotvel);
    body->SetLinearVelocity(state->vel);

  } else {
    if (body) physics.world.DestroyBody(body);
  }
}

void Plane::readFromBody() {
}

Plane::Plane(Engine &engine, PlaneTuning tuning) :
    engine(engine), physics(engine.physics), tuning(tuning) { }

void Plane::spawn(b2Vec2 pos, float rot) {
  state = PlaneState(physics, tuning, pos, rot);
}

void Plane::kill() {
  state = {};
}
}
