#include "physics.h"

namespace sky {

sf::Vector2f Physics::toGameVec(b2Vec2 vec) {
  const float &scalar = settings.distanceScalar;
  return sf::Vector2f(vec.x, vec.y) * settings.distanceScalar;
}

b2Vec2 Physics::toPhysVec(sf::Vector2f vec) {
  return (1 / settings.distanceScalar) * b2Vec2(vec.x, vec.y);
}

b2Body *Physics::planeBody(b2Vec2 dims) {
  b2BodyDef planeBodyDef;
  b2Body *body = world.CreateBody(&planeBodyDef);

  const float &scalar = settings.distanceScalar;
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
