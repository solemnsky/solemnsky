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
    class PlaneVital *planeVital;
    class Prop *prop;
  };

  static BodyTag BoundaryTag();
  static BodyTag ObstacleTag(const class MapObstacle &obstacle);
  static BodyTag PlaneTag(class PlaneVital &planeVital);
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
  b2Body *createBody(const b2PolygonShape &shape,
                     const BodyTag &tag, bool isStatic = false);
  void deleteBody(b2Body *const body);

  b2PolygonShape rectShape(const sf::Vector2f &dims);
  b2PolygonShape polygonShape(const std::vector<sf::Vector2f>
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
