/**
 * A prop, a mutable-lifetime (projectile, etc) entity that a Plane can own.
 * Subordinate to a Plane.
 */
#pragma once
#include "physics.h"

namespace sky {

/**
 * Currently just a bullet.
 */
class Prop {
 private:
  class SkyHolder &parent;
  Physics &physics;
  b2Body *const body;

 public:
  Prop() = delete;
  Prop(class SkyHolder &parent,
       const sf::Vector2f &pos,
       const sf::Vector2f &vel);
  ~Prop();

  float lifeTime;
  PhysicalState physical;

  void writeToBody();
  void readFromBody();
  void tick(const float delta);
};

}
