/**
 * solemnsky: the open-source multiplayer competitive 2D plane game
 * Copyright (C) 2016  Chris Gadzinski
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#include "util/printer.hpp"
#include "physics.hpp"
#include "util/methods.hpp"
#include <polypartition.hpp>

namespace sky {

/**
 * BodyTag.
 */

BodyTag::BodyTag(const BodyTag::Type type, Player *player) :
    type(type), plane(nullptr), player(player) { }

BodyTag BodyTag::BoundaryTag() {
  return BodyTag(Type::BoundaryTag);
}

BodyTag BodyTag::ObstacleTag(const MapObstacle &obstacle) {
  BodyTag tag(Type::ObstacleTag);
  tag.obstacle = &obstacle;
  return tag;
}

BodyTag BodyTag::PlaneTag(Plane &plane, Player &player) {
  BodyTag tag(Type::PlaneTag, &player);
  tag.plane = &plane;
  return tag;
}

BodyTag BodyTag::EntityTag(Entity &entity) {
  BodyTag tag(Type::PropTag);
  tag.entity = &entity;
  return tag;
}

/**
 * PhysicsDispatcher.
 */

PhysicsDispatcher::PhysicsDispatcher(PhysicsListener &listener) :
    listener(listener) { }

void PhysicsDispatcher::BeginContact(b2Contact *contact) {
  listener.onBeginContact(
      *((BodyTag *) contact->GetFixtureA()->GetBody()->GetUserData()),
      *((BodyTag *) contact->GetFixtureB()->GetBody()->GetUserData()));
}

void PhysicsDispatcher::EndContact(b2Contact *contact) {
  listener.onEndContact(
      *((BodyTag *) contact->GetFixtureA()->GetBody()->GetUserData()),
      *((BodyTag *) contact->GetFixtureB()->GetBody()->GetUserData()));
}

void PhysicsDispatcher::PreSolve(b2Contact *contact, const b2Manifold *oldManifold) {
  if (!listener.enableContact(
      *((BodyTag *) contact->GetFixtureA()->GetBody()->GetUserData()),
      *((BodyTag *) contact->GetFixtureB()->GetBody()->GetUserData()))) {
    contact->SetEnabled(false);
  }
}

/**
 * Physics::Settings.
 */

Physics::Settings::Settings() :
    velocityIterations(8),
    positionIterations(3),
    distanceScale(100),
    gravity(150),
    fixtureDensity(10) { }

/**
 * Physics.
 */

void Physics::createFixture(const Shape &shape, b2Body &body) {
  switch (shape.type) {
    case Shape::Type::Circle: {
      circleFixture(shape.radius.get(), body);
      break;
    }
    case Shape::Type::Rectangle: {
      rectFixture(shape.dimensions.get(), body);
      break;
    }
    case Shape::Type::Polygon: {
      polygonFixture(shape.vertices, body);
      break;
    }
    default:
      throw enum_error();
  }
}

void Physics::circleFixture(const float radius, b2Body &body) {
  b2CircleShape shape;
  shape.m_radius = radius;
  body.CreateFixture(&shape, settings.fixtureDensity);
}

void Physics::polygonFixture(const std::vector<sf::Vector2f> &vertices, b2Body &body) {
  // I love great APIs!
  std::vector<sf::Vector2f> verts(vertices);
  pp::Poly poly(verts);
  poly.SetOrientation(TPPL_CCW);
  std::list<pp::Poly> decomposed;

  pp::Partition part;
  part.ConvexPartition_HM(&poly, &decomposed);

  for (const auto &piece : decomposed) {
    b2PolygonShape shape;
    b2Vec2 *points = new b2Vec2[piece.GetNumPoints()];
    size_t i = 0;
    for (const auto &vertex : piece.GetPoints()) {
      points[i] = toPhysVec(vertex);
      ++i;
    }
    shape.Set(points, (int32) piece.GetNumPoints());
    delete[] points;
    body.CreateFixture(&shape, settings.fixtureDensity);
  }
}

void Physics::rectFixture(const sf::Vector2f &dimensions, b2Body &body) {
  b2PolygonShape shape;
  const auto bdims = toPhysVec(dimensions);
  shape.SetAsBox(bdims.x / 2.0f, bdims.y / 2.0f);
  body.CreateFixture(&shape, settings.fixtureDensity);
}

Physics::Physics(const Map &map, PhysicsListener &listener) :
    world({0, Settings().gravity / Settings().distanceScale}),
    converter(listener),
    dims(map.getDimensions()) {
  // world boundaries
  b2Body *body;
#define PAIR std::pair<sf::Vector2f, b2Vec2>
  for (const PAIR &vec :
      {PAIR({dims.x, 1}, toPhysVec({dims.x / 2, dims.y})),
       PAIR({dims.x, 1}, toPhysVec({dims.x / 2, 0})),
       PAIR({1, dims.y}, toPhysVec({dims.x, dims.y / 2})),
       PAIR({1, dims.y}, toPhysVec({0, dims.y / 2}))
      }) {
    body = createBody(Shape::Rectangle(vec.first), BodyTag::BoundaryTag(), false, true);
    body->SetTransform(vec.second, 0);
  }
#undef PAIR

  // Create obstacles from map.
  for (const auto &obstacle : map.getObstacles()) {
    body = createBody(Shape::Polygon(obstacle.localVertices),
                      BodyTag::ObstacleTag(obstacle), false, true);
    body->SetTransform(toPhysVec(obstacle.pos), 0);
  }

  // Set listener pipeline.
  world.SetContactListener(&converter);

  appLog("Instantiated Physics.", LogOrigin::Engine);
}

Physics::~Physics() {
  for (b2Body *body = world.GetBodyList();
       body != nullptr;
       body = body->GetNext()) {
    delete (BodyTag *) body->GetUserData();
  }
}

void Physics::tick(const TimeDiff delta) {
  world.ClearForces();
  world.Step(delta,
             settings.velocityIterations, settings.positionIterations);
}

sf::Vector2f Physics::toGameVec(const b2Vec2 &vec) const {
  return {vec.x * settings.distanceScale,
          vec.y * settings.distanceScale};
}

b2Vec2 Physics::toPhysVec(const sf::Vector2f &vec) const {
  return {vec.x / settings.distanceScale,
          vec.y / settings.distanceScale};
}

float Physics::toGameDistance(const float x) const {
  return x * settings.distanceScale;
}

float Physics::toPhysDistance(const float x) const {
  return x / settings.distanceScale;
}

b2Body *Physics::createBody(const Shape &shape,
                            const BodyTag &tag,
                            bool isBullet,
                            bool isStatic) {
  b2BodyDef def;
  def.fixedRotation = false;
  def.bullet = isBullet;
  def.type = isStatic ? b2_staticBody : b2_dynamicBody;

  b2Body *body = world.CreateBody(&def);
  body->SetUserData(new BodyTag(tag));
  createFixture(shape, *body);
  return body;
}

void Physics::deleteBody(b2Body *const body) {
  if (body) {
    delete (BodyTag *) body->GetUserData();
    world.DestroyBody(body);
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

void Physics::setGravity(const float gravity) {
  world.SetGravity(
      b2Vec2(0, (settings.gravity / settings.distanceScale) * gravity));
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
                                    b2Body *const body) const {
  body->SetLinearVelocity(physics.toPhysVec(vel));
  body->SetAngularVelocity(toRad(rotvel));
  body->SetTransform(physics.toPhysVec(pos), toRad(rot));
}

void PhysicalState::writeToBody(const Physics &physics,
                                b2Body *const body) const {
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
