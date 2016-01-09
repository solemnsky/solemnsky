/**
 * This represents a physical world, with Box2D at the bottom.
 *
 * Box2D has a few quirks and sharp edges that are convenient to encapsulate.
 * Keeping track of the distance scalar (Box2D uses values that are sane in the
 * metric system, our game engine uses values in the 1600x900 order of
 * magnitude...) and creating our bodies without writing a disarmingly large
 * amount of code for example.
 */
#ifndef SOLEMNSKY_PHYSICS_H
#define SOLEMNSKY_PHYSICS_H

#include "base/util.h"

namespace sky {
/**
 * Physics manager, thin and transparent wrapper over Box2D.
 */
class Physics {
public:
  struct Settings {
    float distanceScalar = 100; // pixels per meters
    int velocityIterations = 7;
    int positionIterations = 7;
    float gravity = 200; // pixels per second^2

    Settings() { }
  };

  const Settings settings;
  sf::Vector2f dims;

  b2World world;

  Physics(const sf::Vector2f &dims, const Settings settings = {});
  ~Physics();

  /**
   * Conversion methods, bridging the gap between our engine and box2d.
   */
  sf::Vector2f toGameVec(b2Vec2 vec);
  b2Vec2 toPhysVec(sf::Vector2f vec);
  float inline toGameDistance(float x);
  float inline toPhysDistance(float x);

  /**
   * Making physical stuff.
   */
  b2Body *createBody(const sf::Vector2f &pos, bool dynamic = false);
  b2Fixture *addRectFixture(b2Body *body, const sf::Vector2f &dims);
  b2Fixture *addCircleFixture(b2Body *body, float rad);
  void clrBody(b2Body *body);

  b2Body *planeBody(const sf::Vector2f &dims);

  /**
   * Doing physical stuff.
   */
  void setRotVel(b2Body *body, float rotVel);
  void setVel(b2Body *body, const sf::Vector2f &vel);

  /**
   * Simulating.
   */
  void tick(const float delta); // in seconds
};

float inline Physics::toPhysDistance(float x) {
  return x / settings.distanceScalar;
}

float inline Physics::toGameDistance(float x) {
  return x * settings.distanceScalar;
}

}

#endif //SOLEMNSKY_PHYSICS_H
