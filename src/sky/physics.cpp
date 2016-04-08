#include "physics.h"
#include "util/methods.h"

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

sf::Vector2f Physics::toGameVec(b2Vec2 vec) const {
  return {vec.x * settings.distanceScale,
          vec.y * settings.distanceScale};
}

b2Vec2 Physics::toPhysVec(sf::Vector2f vec) const {
  return {vec.x / settings.distanceScale,
          vec.y / settings.distanceScale};
}

float Physics::toGameDistance(float x) const {
  return x * settings.distanceScale;
}

float Physics::toPhysDistance(float x) const {
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

void Physics::approachRotVel(b2Body *body, float rotvel) const {
  body->ApplyAngularImpulse(
      body->GetInertia() * (toRad(rotvel) - body->GetAngularVelocity()),
      true);
}

void Physics::approachVel(b2Body *body, sf::Vector2f vel) const {
  b2MassData data;
  body->GetMassData(&data); // this is just the most horrifying API ever
  body->ApplyLinearImpulse(
      body->GetMass() * (toPhysVec(vel) - body->GetLinearVelocity()),
      data.center + body->GetWorldCenter(), true);
}

/**
 * PhysicalState.
 */

PhysicalState::PhysicalState(const sf::Vector2f &pos,
                             const sf::Vector2f &vel,
                             const Angle rot,
                             const float rotvel) :
    pos(pos), vel(vel), rot(rot), rotvel(rotvel) { }

void PhysicalState::hardWriteToBody(const Physics &physics,
                                    b2Body *const body) {
  body->SetLinearVelocity(physics.toPhysVec(vel));
  body->SetAngularVelocity(toRad(rotvel));
  body->SetTransform(physics.toPhysVec(pos), toRad(rot));
}

void PhysicalState::writeToBody(const Physics &physics, b2Body *const body) {
  // TODO: use proper motors / motor joints for this instead of impulses
  physics.approachVel(body, vel);
  body->SetTransform(physics.toPhysVec(pos), toRad(rot));
  physics.approachRotVel(body, rotvel);
}

void PhysicalState::readFromBody(const Physics &physics,
                                 const b2Body *const body) {
  pos = physics.toGameVec(body->GetPosition());
  vel = physics.toGameVec(body->GetLinearVelocity());
  rot = toDeg(body->GetAngle());
  rotvel = toDeg(body->GetAngularVelocity());
}

}
