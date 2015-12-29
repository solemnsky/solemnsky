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
  Physics(float scalar) : world(b2Vec2(0, 0)), scalar(scalar),
                          invScalar(1 / scalar) {
    app_log(LogType::Notice, "Instantiated physics."); // necessary logging
  }

  b2World world;

  const float scalar;
  const float invScalar;

  /**
   * Creating stuff.
   */
  b2Body *planeBody(b2Vec2 dims);
};

}

#endif //SOLEMNSKY_PHYSICS_H
