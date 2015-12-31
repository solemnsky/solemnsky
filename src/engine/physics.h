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
    const float distanceScalar = 100; // pixels per meters
    const int velocityIterations = 7;
    const int positionIterations = 7;

    Settings() { }
  };

  Physics(float scalar, Settings settings = Settings{})
      : world(b2Vec2(0, 0)),
        settings(settings), // a bit too much copying for my taste, oh well
        invDistanceScalar(1 / settings.distanceScalar) {

    app_log(LogType::Notice, "Instantiated physics."); // necessary logging
  }

  const Settings settings;
  const float invDistanceScalar;

  b2World world;


  /**
   * Creating stuff.
   */
  b2Body *planeBody(b2Vec2 dims);
  void tick(const float delta);
};

}

#endif //SOLEMNSKY_PHYSICS_H
