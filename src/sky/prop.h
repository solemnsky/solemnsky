/**
 * A prop, a mutable-lifetime (projectile, etc) that a Plane can own.
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
  class Sky &parent;
  Physics &physics;
  b2Body *body;

 public:
  Prop() = delete;
  Prop(class Sky &parent,
       const sf::Vector2f &pos);
  ~Prop();

  Prop(const Prop &) = delete;
  Prop &operator=(const Prop &) = delete;
  Prop(Prop &&);

  float lifeTime;
  PhysicalState physical;

  void writeToBody();
  void readFromBody();
  void tick(const float delta);
};

}
