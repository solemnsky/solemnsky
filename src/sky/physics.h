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
    float gravity = 50; // pixels per second^2

    Settings() { }
  };

  const Settings settings;
  sf::Vector2f dims;

  b2World world;

  Physics(sf::Vector2f dims, Settings settings = Settings{});

  /**
   * Conversion methods, bridging the gap between our engine and box2d.
   */
  sf::Vector2f toGameVec(b2Vec2 vec);
  b2Vec2 toPhysVec(sf::Vector2f vec);

  /**
   * Creating physical stuff.
   */
  b2Body *createBody(sf::Vector2f pos);
  b2Fixture *addRectFixture(b2Body *, sf::Vector2f dims);

  b2Body *planeBody(sf::Vector2f dims);

  /**
   * Simulating.
   */
  void tick(const float delta); // in seconds
};
}

#endif //SOLEMNSKY_PHYSICS_H
