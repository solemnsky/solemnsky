/**
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

    Settings() { }
  };

  Physics(Settings settings = Settings{})
      : world(b2Vec2(0, 0)),
        settings(settings) {
    app_log(LogType::Notice, "Instantiated physics."); // necessary logging
  }

  /**
   * Settings and conversion helpers.
   */
  const Settings settings;
  sf::Vector2f toGameVec(b2Vec2 vec);
  b2Vec2 toPhysVec(sf::Vector2f vec);

  /**
   * Box2d world.
   */
  b2World world;

  /**
   * Creating physical stuff.
   */
  b2Body *planeBody(b2Vec2 dims);
  void tick(const float delta); // in seconds
};

}

#endif //SOLEMNSKY_PHYSICS_H
