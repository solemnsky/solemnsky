#include "physics.h"

namespace sky {

b2Body *Physics::planeBody(b2Vec2 dims) {
  b2BodyDef planeBodyDef;
  b2Body *body = world.CreateBody(&planeBodyDef);

  const float scalar = settings.distanceScalar;
  b2PolygonShape planeShape;
  planeShape.SetAsBox(scalar * dims.x, scalar * dims.y);
  b2FixtureDef planeFixture;
  planeFixture.shape = &planeShape; // it will be cloned right off the bat
  // (the pointer won't be dangling)

  planeFixture.restitution = 0; // etc etc, more settings go here

  body->CreateFixture(&planeFixture);

  return body;
}

void Physics::tick(const float delta) {
  world.Step(delta, settings.velocityIterations, settings.positionIterations);
}
}
