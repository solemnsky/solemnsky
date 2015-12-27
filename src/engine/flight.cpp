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
}

void Plane::readFromBody() {
}

Plane::Plane(Engine *engine, const PlaneState state) :
    engine(engine),
    physics(&engine->physics),
    state(state) {
}
}
