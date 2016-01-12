#include "physics.h"

namespace sky {

Physics::Physics() : world({0, 10}) {
  b2Body *body;

  // make floor
  body = rectBody({1600, 1}, true);
  body->SetTransform(toPhysVec({800, 900}), 0);

  body = rectBody({1600, 1}, true);
  body->SetTransform(toPhysVec({800, 0}), 0);

  body = rectBody({1, 900}, true);
  body->SetTransform(toPhysVec({1600, 450}), 0);

  body = rectBody({1, 900}, true);
  body->SetTransform(toPhysVec({0, 450}), 0);
}


void Physics::tick(const float delta) {
  world.Step(delta, settings.velocityIterations, settings.positionIterations);
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

float Physics::toDeg(float rad) {
  constexpr float factor((const float) (180 / 3.14159));
  return rad * factor;
}

float Physics::toRad(float deg) {
  constexpr float factor((const float) (3.14159 / 180));
  return deg * factor;
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
