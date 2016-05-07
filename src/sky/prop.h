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
  friend class Participation;
 private:
  // State.
  Physics &physics;
  b2Body *const body;
  PhysicalState physical;
  float lifetime;

  // Sky API.
  void writeToBody();
  void readFromBody();
  void tick(const float delta);

 public:
  Prop() = delete;
  Prop(Physics &physics,
       const sf::Vector2f &pos,
       const sf::Vector2f &vel);
  ~Prop();

  // User API.
  const PhysicalState &getPhysical() const;
  float getLifetime() const;

};

}
