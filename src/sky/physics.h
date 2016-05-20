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
/**
 * Physics manager; wraps box2d.
 */
#pragma once
#include <Box2D/Box2D.h>
#include "util/types.h"
#include "map.h"

namespace sky {

/**
 * Some metadata attached to a body in the simulation.
 */
struct BodyTag {
  enum class Type {
    BoundaryTag, ObstacleTag, PlaneTag, PropTag
  };

  const Type type;

 private:
  BodyTag(const Type type);

 public:
  union {
    const class MapObstacle *obstacle;
    class Plane *plane;
    class Prop *prop;
  };

  static BodyTag BoundaryTag();
  static BodyTag ObstacleTag(const class MapObstacle &obstacle);
  static BodyTag PlaneTag(Plane &plane);
  static BodyTag PropTag(class Prop &prop);
};

/**
 * Interface to listen to physical events.
 */
class PhysicsListener {
 protected:
  friend class PhysicsDispatcher;
  virtual void onBeginContact(const BodyTag &body1, const BodyTag &body2) = 0;
  virtual void onEndContact(const BodyTag &body1, const BodyTag &body2) = 0;
};

/**
 * Listens to the b2World and dispatches to PhysicsListener.
 */
class PhysicsDispatcher: public b2ContactListener {
 private:
  PhysicsListener &listener;

 public:
  PhysicsDispatcher(PhysicsListener &listener);

  // b2ContactListener
  virtual void BeginContact(b2Contact *contact) override;
  virtual void EndContact(b2Contact *contact) override;

};

/**
 * A physical world.
 */
class Physics {
 private:
  const struct Settings {
    Settings() { }

    int velocityIterations = 8, positionIterations = 3;
    float distanceScale = 100;
    float gravity = 150;
  } settings;

  b2World world;
  PhysicsListener &listener;
  PhysicsDispatcher converter;

 public:
  Physics() = delete;
  Physics(const Map &map, PhysicsListener &listener);
  ~Physics();

  const sf::Vector2f dims;

  void tick(const float delta);

  /**
   * Conversion.
   */
  sf::Vector2f toGameVec(b2Vec2 vec) const;
  b2Vec2 toPhysVec(sf::Vector2f vec) const;
  float toGameDistance(float x) const;
  float toPhysDistance(float x) const;

  /**
   * Managing bodies.
   */
  b2Body *createBody(const b2Shape &shape,
                     const BodyTag &tag, bool isStatic = false);
  void deleteBody(b2Body *const body);

  b2PolygonShape rectShape(const sf::Vector2f &dims);
  b2PolygonShape polygonShape(const std::vector<sf::Vector2f>
                                     &verticies);
  b2ChainShape chainLoopShape(const std::vector<sf::Vector2f>
                                     &verticies);

  // approach rotational / linear velocities by applying impulses
  void approachRotVel(b2Body *body, float rotvel) const;
  void approachVel(b2Body *body, sf::Vector2f vel) const;
};

/**
 * The physical state of a body.
 */
struct PhysicalState {
  PhysicalState() = default; // packing
  PhysicalState(const sf::Vector2f &pos, const sf::Vector2f &vel,
                const Angle rot, const float rotvel);

  template<typename Archive>
  void serialize(Archive &ar) {
    ar(pos, vel, rot, rotvel);
  }

  sf::Vector2f pos, vel;
  Angle rot;
  float rotvel;

  void hardWriteToBody(const Physics &physics, b2Body *const body) const;
  void writeToBody(const Physics &physics, b2Body *const body) const;
  void readFromBody(const Physics &physics, const b2Body *const body);
};

}
