#include "physics.h"

namespace sky {

Physics::Physics(const Map &map) :
    dims(map.dimensions),
    world({0, Settings().gravity / Settings().distanceScale}) {
  CTOR_LOG("physics");

  b2Body *body;

  body = rectBody({dims.x, 1}, true);
  body->SetTransform(toPhysVec({dims.x / 2, dims.y}), 0);

  body = rectBody({dims.x, 1}, true);
  body->SetTransform(toPhysVec({dims.x / 2, 0}), 0);

  body = rectBody({1, dims.y}, true);
  body->SetTransform(toPhysVec({dims.x, dims.y / 2}), 0);

  body = rectBody({1, dims.y}, true);
  body->SetTransform(toPhysVec({0, dims.y / 2}), 0);
}

Physics::~Physics() {
  DTOR_LOG("physics");
}

void Physics::tick(const float delta) {
  world.ClearForces();
  world.Step(delta, settings.velocityIterations, settings.positionIterations);
}

sf::Vector2f Physics::toGameVec(b2Vec2 vec) {
  return {vec.x * settings.distanceScale,
          vec.y * settings.distanceScale};
}

b2Vec2 Physics::toPhysVec(sf::Vector2f vec) {
  return {vec.x / settings.distanceScale,
          vec.y / settings.distanceScale};
}

float Physics::toGameDistance(float x) {
  return x * settings.distanceScale;
}

float Physics::toPhysDistance(float x) {
  return x / settings.distanceScale;
}


b2Body *Physics::rectBody(sf::Vector2f dims, bool isStatic) {
  b2Body *body;
  const b2Vec2 &&bdims = toPhysVec(dims);

  b2BodyDef def;
  def.fixedRotation = false;
  def.type = isStatic ? b2_staticBody : b2_dynamicBody;

  b2PolygonShape shape;
  shape.SetAsBox(bdims.x / 2, bdims.y / 2);
  body = world.CreateBody(&def);
  body->CreateFixture(&shape, 10.0f);

  return body;
}

void Physics::clrBody(b2Body *&body) {
  if (body) {
    world.DestroyBody(body);
    body = nullptr;
  }
}

void Physics::approachRotVel(b2Body *body, float rotvel) {
  body->ApplyAngularImpulse(
      body->GetInertia() * (toRad(rotvel) - body->GetAngularVelocity()),
      true);
}

void Physics::approachVel(b2Body *body, sf::Vector2f vel) {
  b2MassData data;
  body->GetMassData(&data); // this is just the most horrifying API ever
  body->ApplyLinearImpulse(
      body->GetMass() * (toPhysVec(vel) - body->GetLinearVelocity()),
      data.center + body->GetWorldCenter(), true);
}

}
