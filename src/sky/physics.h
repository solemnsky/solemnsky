/**
 * Physics manager; wraps box2d.
 */
#pragma once
#include <Box2D/Box2D.h>
#include "util/types.h"
#include "map.h"

namespace sky {

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

 public:
  Physics() = delete;
  Physics(const Map &map);
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
  // construct a body with a rectangular fixture
  b2Body *rectBody(sf::Vector2f dims, bool isStatic = false);

  // clear a body pointer
  void clrBody(b2Body *&body);

  // approach angular / linear velocities with impulses (to participate
  // in the simulation correctly)
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
    ar(pos, rot, rotvel);
  }

  sf::Vector2f pos, vel;
  Angle rot;
  float rotvel;

  void hardWriteToBody(const Physics &physics, b2Body *const body);
  void readFromBody(const Physics &physics, const b2Body *const body);
  void writeToBody(const Physics &physics, b2Body *const body);
};

}
