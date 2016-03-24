/**
 * Physics manager; wraps box2d.
 */
#pragma once
#include <Box2D/Box2D.h>
#include "map.h"

namespace sky {

/**
 * Manages a Box2D world, and provides a wrapped API so we don't have to deal
 * with actual C-style Box2D in the rest of the code.
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
  Physics(const Map &map);
  ~Physics();

  const sf::Vector2f dims;

  void tick(const float delta);

  /**
   * Conversion.
   */
  sf::Vector2f toGameVec(b2Vec2 vec);
  b2Vec2 toPhysVec(sf::Vector2f vec);
  float toGameDistance(float x);
  float toPhysDistance(float x);

  /**
   * Managing bodies.
   */
  // construct a body with a rectangular fixture
  b2Body *rectBody(sf::Vector2f dims, bool isStatic = false);

  // clear a body pointer
  void clrBody(b2Body *&body);

  // approach angular / linear velocities with impulses (to participate
  // in the simulation correctly)
  void approachRotVel(b2Body *body, float rotvel);
  void approachVel(b2Body *body, sf::Vector2f vel);
};

}
