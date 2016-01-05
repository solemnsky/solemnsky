#include "physics.h"

namespace sky {

Physics::Physics(sf::Vector2f dims, Settings settings)
    : world({0, settings.gravity / settings.distanceScalar}),
      dims(dims),
      settings(settings) {
  b2Vec2 b2dims = toPhysVec(dims);
  float wallWidth = 0.1;

  /**
   * Add walls. (Only runs once.)
   */
  b2BodyDef wallDef;
  b2PolygonShape wallShape;
  b2Body *body;

  body = createBody({0, dims.y / 2}, false);
  addRectFixture(body, {wallWidth / 2, dims.y / 2});

  body = createBody({dims.x, dims.y / 2}, false);
  addRectFixture(body, {wallWidth / 2, dims.y / 2});

  body = createBody({dims.x / 2, 0}, false);
  addRectFixture(body, {dims.x / 2, wallWidth});

  body = createBody({dims.x / 2, dims.y}, false);
  addRectFixture(body, {dims.x / 2, wallWidth});
}

sf::Vector2f Physics::toGameVec(b2Vec2 vec) {
  const float &scalar = settings.distanceScalar;
  return sf::Vector2f(vec.x, vec.y) * settings.distanceScalar;
}

b2Vec2 Physics::toPhysVec(sf::Vector2f vec) {
  return (1 / settings.distanceScalar) * b2Vec2(vec.x, vec.y);
}

b2Body *Physics::createBody(sf::Vector2f pos, bool dynamic) {
  b2BodyDef def;
  def.position = toPhysVec(pos);
  if (dynamic) def.type = b2BodyType::b2_dynamicBody;
  else def.type = b2BodyType::b2_staticBody;
  return world.CreateBody(&def);
}

b2Fixture *Physics::addRectFixture(b2Body *body, sf::Vector2f dims) {
  b2Vec2 bdims = toPhysVec(dims);

  b2PolygonShape shape;
  shape.SetAsBox(bdims.x / 2, bdims.y / 2);
  b2FixtureDef fixture;
  fixture.shape = &shape;
  fixture.density = 1.0f;
  return body->CreateFixture(&fixture);
}

b2Fixture *Physics::addCircleFixture(b2Body *body, float rad) {
  float brad = toPhysDistance(rad);

  b2CircleShape shape;
  shape.m_radius = brad;
  b2FixtureDef fixture;
  fixture.shape = &shape;
  fixture.density = 1.0f;
  return body->CreateFixture(&fixture);
}

b2Body *Physics::planeBody(sf::Vector2f dims) {
  b2BodyDef planeBodyDef;
  b2Body *body = world.CreateBody(&planeBodyDef);

  b2Fixture *planeFixture = addRectFixture(body, dims);
  planeFixture->SetRestitution(0); // etc etc, more settings go here

  return body;
}

void Physics::tick(const float delta) {
  world.Step(delta, settings.velocityIterations, settings.positionIterations);
}

}
