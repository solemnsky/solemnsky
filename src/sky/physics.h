/**
 * Physics manager, wrapping box2d, making our lives easier in small ways.
 */

#ifndef SOLEMNSKY_PHYSICS_H
#define SOLEMNSKY_PHYSICS_H

#include <Box2D/Box2D.h>
#include "base/util.h"

namespace sky {

class Physics {
private:
  b2World world;

public:
  const struct Settings {
    int velocityIterations = 8, positionIterations = 3;
    float distanceScale = 100;
    Settings() = default;
  } settings;

  Physics();

  void tick(const float delta);

  /**
   * Conversion.
   */
  sf::Vector2f toGameVec(b2Vec2 vec);
  b2Vec2 toPhysVec(sf::Vector2f vec);
  float toGameDistance(float x);
  float toPhysDistance(float x);
  float toDeg(float rad); // SF uses degrees, box2D uses radians
  float toRad(float deg);

  /**
   * Managing bodies.
   */
  b2Body *rectBody(sf::Vector2f dims, bool isStatic = false);
  void clrBody(b2Body *body);

  void approachRotVel(b2Body *body, float rotvel);
  void approachVel(b2Body *body, sf::Vector2f vel);
};

}

#endif //SOLEMNSKY_PHYSICS_H
